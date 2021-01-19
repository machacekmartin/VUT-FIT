#include "ipk-scan.h"

/*
 * Declarations of global variables
*/
int currentPort;
string currentProtocol;
pcap_t *handler;
bool wasFiltered = false;

/*
 * Error function for printing given string and exiting under given erro number
 * @param   message     message that will be printed right before exititng program
 * @param   num         error number
*/
void exitErr(string message, int num){
    cout << "\nERROR:\t" << message << "\n";
    cout << "\tUse -help for instructions\n\n";
    exit(num);
}

/*
 * Each cycle in main function, an index is written into vector of taken positions
 * To determine position of destination address, this function goes through each position
 * and checks what position isnt taken yet. That position is the position of demanded address
 * @param   s       arguments count
 * @param   taken   vector("array") of already taken positions
 *
 * @return  position of demanded address
*/
int getAddrPos(int s, vector<int> taken) {
    for (int i = 0; i <= s; i++) {
        if ((find(taken.begin(), taken.end(), i)) == taken.end()) return i;
    }
    exitErr("Address not found",101);
}

/*
 * Takes a string, and splits ip up into multiple pieces by given delimiter
 * This function only returns vector of integers values and is intended to split
 * strings like f.e. "19,20,80,999,200" or f.e. "20-800".
 * @param   s           given string to be split
 * @param   delimiter   character by which the string will be split
 *
 * @return  vector("array") of integer values
*/
vector<int> makeArrayFromString(string s, char delimiter) {
    vector<int> arr;
    string buffer{""};
    for (auto x : s) {
        if (x != delimiter) buffer += x;
        else if (x == delimiter && buffer != "") {
            arr.push_back(stoi(buffer));
            buffer = "";
        }
    }
    if (buffer != "") arr.push_back(stoi(buffer));
    return arr;
}

/*
 * Checks if the given string matches at least one regex
 * Extracts ports from given string and fils an array with actual ports
 * @param   s   string taken from argument, from which we want to make array of ports
 *
 * @return  vector ("array") of actual ports
*/
vector<int> extractPorts(string s) {
    vector<int> ports;
    int first;
    int last;
    if (regex_match(s, regex("^([0-9]{1,5})-([0-9]{1,5})$"))) {
        ports = makeArrayFromString(s, '-');
        first = ports[0];
        last = ports[1];
        if (first > last) exitErr("Wrong port range format. Range must be ascending", 77);
        ports.clear();
        for (int i = first; i <= last; i++) ports.push_back(i);
    }
    else if (regex_match(s, regex("^([0-9]+)(,[0-9]+)*$"))) {
        ports = makeArrayFromString(s, ',');
    }
    else exitErr("Wrong ports input", 77);

    for (int i = 0; i < ports.size(); i++) {
        if (ports.at(i) > 65535 || ports.at(i) < 0) exitErr("Supported port range is only 1 - 65535", 78);
    }
    return ports;
}

/*
 * Finds all available network interfaces with usage of getifaddrs function
 *
 * @return  array of all available network interfaces
*/
ifaddrs *getAllInterfaces(){
    ifaddrs *interfaces;
    if (getifaddrs(&interfaces) < 0) exitErr("There arent any interfaces or something went wrong with looking for them", 76);
    return interfaces;
}

/*
 * Find the first non-loopback interface by first, getting every available interface
 * then cycling through each interface and checking their flag - IFF_LOOPBACK
 *
 * @return  first non-loopback interface
*/
ifaddrs *getFirstNonLoopbackI(){
    struct ifaddrs *interfaces = getAllInterfaces();
    struct ifaddrs *temp;
    for(temp=interfaces; temp != NULL; temp=temp->ifa_next){
        if (temp->ifa_addr->sa_family == AF_INET){
            int iIsLoopBack = (0 != (temp->ifa_flags & IFF_LOOPBACK));
            if (iIsLoopBack == 1){
            }
            else{
                return temp;
            }
        }
    }
    exitErr("There isnt any non-loopback interface to choose from", 75);
}

/*
 * Finds interface by given name (string)
 * @param   interf  string name of demanded interface
 *
 * @return  interface with given name
*/
ifaddrs *assignInterf(string interf){
    struct ifaddrs *interfaces = getAllInterfaces();
    struct ifaddrs *temp;
    for(temp=interfaces; temp; temp=temp->ifa_next){
        if (temp->ifa_addr->sa_family == AF_INET){
            if (temp->ifa_name == interf){
                return temp;
            }
        }
    }
    exitErr("There isnt any interface that shares name with your choice", 74);
}

/*
 * Calculates checksum for given data - used in filling ip header and tcpheader
 * @param   data    data from which we want to calculate the checksum
 * @param   s       size
 *
 * @return  result of checksum
 *
 * This function was constructed with help of these articles/posts/websites:
 * These websites also mention authors & licences
 * ----------------------------------------------------------------------------
 *      - https://locklessinc.com/articles/tcp_checksum
 *      - https://tools.ietf.org/html/rfc793
 *          - page 14+
 *      - https://stackoverflow.com/questions/8845178/c-programming-tcp-checksum
 *          - answer from user synthesizerpatel
 *      - https://www.rfc-editor.org/rfc/rfc1071.txt
 *------------------------------------------------------------------------------
*/
uint16_t checkSum(const uint16_t* data, unsigned int s){
    uint32_t sum = 0;
    for (; s > 1; s -= 2){
        sum += *data++;
    }
    if (s == 1) sum += *(unsigned char*) data;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    return ~sum;
}

/*
 * Prints the result for the user
 * @param   state   state of processed port
*/
void printState(string state){
    cout << currentPort << "/" << currentProtocol << "   \t" << state << "\n";
}

/*
 * This functions just calls the print of the port processing result
 * and breaks the loop which would otherwise search for next packets
*/
void processICMP(){
    printState("Closed");
    pcap_breakloop(handler);
}

/*
 * Recreates received TCP packet into form from which we can determine something
 * First, function recreates the ip header of packet, then tcpheader of packet
 * Tcpheader can be then used to access usefull information like ack or rst flags
 * Function then determines the port state by looking at the ack and rst flags
 * and breaks the packet capturing loop if needed
 * @param   buff    received packet which will be transformed into needed tcp format
 *
 * This function was constructed with help of these articles/posts/websites:
 * These websites also mention authors & licences
 * ----------------------------------------------------------------------------
 *      - https://www.tcpdump.org/pcap.html
 *          - at the bottom
 *      - https://www.exploit-db.com/papers/13189
 *          - 7.1.5 Pseudo Header
 *
 *------------------------------------------------------------------------------
*/
void processTCP(const u_char *buff){
    unsigned short iphLen;
    struct iphdr *iph = (struct iphdr *)( buff  + sizeof(struct ethhdr) );
    iphLen = iph->ihl*4;
    struct tcphdr *tcph=(struct tcphdr*)(buff + iphLen + sizeof(struct ethhdr));

    if ((unsigned int)tcph->ack == 1 && (unsigned int)tcph->rst == 1){
        printState("Closed");
        pcap_breakloop(handler);
    }
    else if ((unsigned int)tcph->ack == 1){
        printState("Opened");
        pcap_breakloop(handler);
    }
}

/*
 * Timeout function called by alarm signal set for each packet capture
 * Prints out port state and breaks loop
 * @param   s   seconds
*/
void handle(int s){
    pcap_breakloop(handler);
    if (currentProtocol == "udp") printState("Opened");
    else{
        if (wasFiltered){
            printState("Filtered");
        }
        else{
            wasFiltered = true;
        }
    }
}

/*
 * This function is called for each found packet
 * Recreates packet ip header and checks the header protocol from captured packet
 * In our case, we need ICMP and TCP packets only, throw everything else away
 * @PARAM   FUNCTION HAS PRE-DEFINED PARAMETERS FROM PCAP LIBRARY
*/
void handlePacket(u_char *args, const struct pcap_pkthdr *header, const u_char *packet){
    struct iphdr *iph = (struct iphdr*)(packet + sizeof(struct ethhdr));
    switch(iph->protocol){
        case 1:
            processICMP();
            break;
        case 6:
            processTCP(packet);
            break;
        default:
            break;
    }
}

/*
 * Fills information of global variable handler, which is used to
 * handle and filter out packets
 * @param   i       interface under which the handler will operate
 * @param   time    timeout of looking for packet
 * @param   address address of destination to communicate with
 *
 * This function was constructed with help of these articles/posts/websites:
 * These websites also mention authors & licences
 * ----------------------------------------------------------------------------
 *      - https://www.devdungeon.com/content/using-libpcap-c
 *          - this whole page helped me understand libcap and how to capture packets
 *------------------------------------------------------------------------------
*/
void createHandler(ifaddrs *i, int time, char* address){
    struct bpf_program compiledFilter;
    bpf_u_int32 interfaceMask;
    bpf_u_int32 interfaceIp;
    char errors[PCAP_ERRBUF_SIZE];
    char filterDesc[64];

    if (currentProtocol == "tcp") sprintf(filterDesc, "host %s and dst port 44444", address);
    else if (currentProtocol == "udp") sprintf(filterDesc, "host %s", address);

    if ((handler = pcap_open_live(i->ifa_name, 65535, 1, time, errors)) == NULL) exitErr("Could not create new packet handler", 73);
    if ((pcap_lookupnet(i->ifa_name, &interfaceIp, &interfaceMask, errors)) < 0) exitErr("Could not assign IP or Mask to packet handler", 72);
    if ((pcap_compile(handler, &compiledFilter, filterDesc, 0, interfaceMask)) < 0) exitErr("Could not compile handle filter", 71);
    if ((pcap_setfilter(handler, &compiledFilter)) < 0) exitErr("Could not set filter to handler", 70);
}

/*
 * Fills the given ip header with actual must-have information
 * @param   iph         ip header to be filled
 * @param   destAddr    destination address
 * @param   srcAddr     source address
 * @param   packet      packet for calculating checksum
 *
 * @return  filled ip header
 *
*/
iphdr fillIPHeaderTCP(iphdr *iph, u_int32_t destAddr, u_int32_t srcAddr, char packet[512]){
    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 0;
    iph->tot_len = sizeof(struct iphdr) + sizeof(struct tcphdr);
    iph->id = htonl(54321);
    iph->frag_off = 0x00;
    iph->ttl = 0xFF;
    iph->protocol = IPPROTO_TCP;
    iph->check  = 0;
    iph->saddr = srcAddr;
    iph->daddr = destAddr;
    iph->check  = checkSum((unsigned short *) packet, iph->tot_len);

    return *iph;
}

/*
 * Fills the given tcp header with actual must-have information
 * @param   tcph    tcp header to be filled
 *
 * @return  filled tcp header
*/
tcphdr fillTCPHeader(tcphdr *tcph){
    tcph->source = htons(44444);
    tcph->dest = htons(currentPort);
    tcph->seq = 0x0;
    tcph->ack_seq = 0x0;
    tcph->doff = 5;
    tcph->syn = 1;
    tcph->window = htons(155);
    tcph->check = 0;
    tcph->urg_ptr = 0;

    return *tcph;
}

/*
 * Fills the given sockaddr_in variable with important information
 * @param   sin     sockaddr_in variable to filled
 * @param   dstAddr destination address
 *
 * @return  filled sockaddr_in variable
*/
sockaddr_in fillSin(sockaddr_in sin, u_int32_t dstAddr){
    sin.sin_family = AF_INET;
    sin.sin_port = htons(currentPort);
    sin.sin_addr.s_addr = dstAddr;

    return sin;
}

/*
 * Fills the given pseudo header with important information
 * @param   psh         pseudo header to be filled
 * @param   srcAddr     adress of source client
 * @param   dstAddr     destination address
 *
 * @return  filled pseudo header
*/
pseudo_header fillPseudoHeader(pseudo_header psh, u_int32_t srcAddr, u_int32_t dstAddr){
    psh.source_address = srcAddr;
    psh.dest_address = dstAddr;
    psh.zero = 0;
    psh.protocol = IPPROTO_TCP;
    psh.tcp_length = htons(sizeof(struct tcphdr));

    return psh;
}

/*
 * Connects to a given address with chosen interface via TCP protocol
 * This function does all the connection stuff, first, declares needed variables
 * creates raw socket, fills ip, tcp pseudo headers, calculates checksum
 * applies packet capture filter and then sends the created packet and catches response packets
 * for each received packet, there is a time limit during which the packets can be caught
 * @param   i           interface through which we want to send packets
 * @param   address     destination device with which we will communicate
 *
 * This function was constructed with help of these articles/posts/websites:
 * These websites also mention authors & licences
 * ------------------------------------------------------------------------------
 *      - https://www.tenouk.com/Module43a.html
 *              - helped me understand libcap and how to capture packets
 *      - https://www.tenouk.com/Module43.html
 *      - www.cplusplus.com/forum/general/7109/
 *      - https://www.binarytides.com/raw-sockets-c-code-linux/
 *      - www.tcpipguide.com/free/t_TCPChecksumCalculationandtheTCPPseudoHeader-2.htm
 *      - https://stackoverflow.com/questions/40795772/cant-recieve-packets-to-raw-socket
 *              - answer from user Ricardo Biehl Pasquali
 *              - inspired portion is assignment of sin variable
 *      - https://en.ikipedia.org/wiki/Network_socket
 * ------------------------------------------------------------------------------
*/
void connectViaTCP(ifaddrs *i, char* address){
    currentProtocol = "tcp";

    struct hostent *target;
    u_int16_t srcPort;
    u_int32_t srcAddr;
    u_int32_t dstAddr;

    if ((target = gethostbyname(address)) == NULL) exitErr("Could not find host name", 69);

    srcPort = 44444;
    dstAddr = inet_addr(inet_ntoa(**(in_addr**)target->h_addr_list));
    srcAddr = inet_addr(inet_ntoa(((struct sockaddr_in *) i->ifa_addr)->sin_addr));

    // socket, packet, iph header, tcpheader, sin, pseudo header declarations
    int cSocket;
    char packet[512], *pseudoPacket;
    struct iphdr *iph = (struct iphdr *) packet;
    struct tcphdr *tcph = (struct tcphdr *) (packet + sizeof (struct iphdr));
    struct sockaddr_in sin;
    struct pseudo_header psh;

    int opt = 1;
    const int *val = &opt;

    // socket
    if ((cSocket = socket(AF_INET, SOCK_RAW, IPPROTO_TCP)) < 1) exitErr("Could not create new socket", 68);

    memset(packet, 0, sizeof(packet));

    // sin, ipheader, tcpheader, pseudoheader
    sin = fillSin(sin, dstAddr);
    *iph = fillIPHeaderTCP(iph, dstAddr, srcAddr, packet);
    *tcph = fillTCPHeader(tcph);
    psh = fillPseudoHeader(psh, srcAddr, dstAddr);

    // creating important allocations for pseudo packet and filling things like headers with actual important stuff
    pseudoPacket = (char *) malloc((int) (sizeof(struct pseudo_header) + sizeof(struct tcphdr)));
    memset(pseudoPacket, 0, sizeof(struct pseudo_header) + sizeof(struct tcphdr));
    memcpy(pseudoPacket, (char*) &psh, sizeof(struct pseudo_header));
    memcpy(pseudoPacket + sizeof(struct pseudo_header), tcph, sizeof(struct tcphdr));

    // calculation of checksum of tcp header - very important
    tcph->check = (checkSum((unsigned short*) pseudoPacket, (int) (sizeof(struct pseudo_header) + sizeof(struct tcphdr))));

    // let the socket know that it will be used in our needs
    if (setsockopt(cSocket, IPPROTO_IP, IP_HDRINCL, val, sizeof(opt)) < 0) exitErr("Could not set socket opetion", 67);

    // setting up handler filter
    createHandler(i, 0, address);

    // SEND IT ALREADY!
    if(sendto(cSocket, packet, iph->tot_len, 0, (struct sockaddr *) &sin, sizeof(sin)) < 0) exitErr("Sending packet failed", 63);

    alarm(3);
    signal(SIGALRM, handle);

    // PICKUP PACKETS
    pcap_loop(handler, 2, handlePacket, NULL);
    pcap_close(handler);
    free(pseudoPacket);
}

/*
 * Fills the given ip header with actual must-have information
 * @param   iph     ip header to be filled
 * @param   dst     destination ip
 * @param   src     source ip
 *
 * @return  filled ip header
*/
iphdr fillIPHeaderUDP(iphdr *iph, u_int32_t dst, u_int32_t src){
    iph->ihl      = 5;
    iph->version  = 4;
    iph->tos      = 16;
    iph->tot_len  = sizeof(struct iphdr) + sizeof(struct udphdr);
    iph->id       = htons(54321);
    iph->ttl      = 64;
    iph->protocol = IPPROTO_UDP;
    iph->saddr = src;
    iph->daddr = dst;
    iph->check = 0;

    return *iph;
}

/*
 * Connects to a given address with chosen interface via UDP protocol
 * This function does all the connection stuff, first, declares needed variables
 * creates raw socket, fills ip, udp header
 * applies packet capture filter and then sends the created packet and catches response packets
 * for each received packet, there is a time limit during which the packets can be caught
 * if received packet is ICMP type, port is closed, open/filtered otherwise
 * @param   i           interface through which we want to send packets
 * @param   address     destination device with which we will communicate
 *
 * This function was constructed with help of these articles/posts/websites:
 * These websites also mention authors & licences
 * ------------------------------------------------------------------------------
 *      - https://www.root.cz/clanky/sokety-a-c-raw-soket/
 *
 *      - inspiration taken from already completed function connectViaTCP
 *        after which I understood how to make packet and how to send it much more
 * ------------------------------------------------------------------------------
*/
void connectViaUDP(ifaddrs *i, char *address){
    currentProtocol = "udp";
    struct hostent *target;

    u_int16_t srcPort;
    u_int32_t srcAddr;
    u_int32_t dstAddr;

    if ((target = gethostbyname(address)) == NULL) exitErr("Given host name doesnt exist or has incorrect format", 66);

    srcPort = 44444;
    dstAddr = inet_addr(inet_ntoa(**(in_addr**)target->h_addr_list));
    srcAddr = inet_addr(inet_ntoa(((struct sockaddr_in *) i->ifa_addr)->sin_addr));

    // socket, packet, iph header, udp header, sin declarations
    int cSocket;
    char packet[512];
    struct iphdr *iph = (struct iphdr *) packet;
    struct udphdr *udph = (struct udphdr *) (packet + sizeof(struct iphdr));
    struct sockaddr_in sin;

    int one = 1;
    const int *val = &one;

    memset(packet, 0, sizeof(packet));

    // socket, socket options
    if ((cSocket = socket(PF_INET, SOCK_RAW, IPPROTO_UDP)) < 0) exitErr("Could not create socket", 65);
    if (setsockopt(cSocket, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0) exitErr("Could not add socket option", 64);

    // sin, iph header, udp header assignments
    sin = fillSin(sin, dstAddr);
    *iph = fillIPHeaderUDP(iph, srcAddr, dstAddr);
    udph->source = htons(srcPort);
    udph->dest = htons(currentPort);
    udph->len = htons(sizeof(struct udphdr));

    // setting up handler filter
    createHandler(i, 300, address);

    // SEND IT ALREADY!
    if (sendto(cSocket, packet, iph->tot_len, 0, (struct sockaddr *)&sin, sizeof(sin)) < 0) exitErr("Sending packet failed", 63);

    ualarm(800000,0);
    signal(SIGALRM, handle);

    pcap_loop(handler, 2, handlePacket, NULL);

    pcap_close(handler);
    close(cSocket);
}

/*
 * Calls appropriate function for udp and tpc protocols
 * These functions will be called for each port in array of ports declared earlier
 * @param   pt      vector of integer values (ports) for TCP protocol
 * @param   pu      vector of integer values (ports) for UDP protocol
 * @param   i       interface through which we will connect to the destination
 * @param   address destination address (string) - ip or name
*/
void connectToAddress(vector<int> pt, vector<int> pu, ifaddrs *i, string address){
    char a[address.size() + 1];
    strcpy(a, address.c_str());

    cout << "---------------------------------------------------------\n";
    cout << "Interface used:\t\t" << i->ifa_name << "\n";
    cout << "Destination address:\t" << address << "\n";
    cout << "---------------------------------------------------------\n";
    cout << "PORT\t\tSTATE\n";

    for(int port : pt){
        currentPort = port;
        connectViaTCP(i, a);
        if (wasFiltered){
            connectViaTCP(i, a);
            wasFiltered = false;
        }
    }
    for (int port: pu){
        currentPort = port;
        connectViaUDP(i, a);
    }
}

void printHelp(){
    cout << "HELP \n"
        "\tsudo ./ipk-scan -i <interface> -pu <port-ranges> -pt <port-ranges> <address> \n"
            "\tWhere\n"
            "\t\t ipk-scan\n"
                    "\t\t\ti s generated file from make command\n\n"
            "\t\t -i <interface>\n"
                    "\t\t\t is optinal command for specifying network interface, under which program will send / recieve packets\n"
                    "\t\t\t and <interface> is name of interface. If not present, first IEEE 802 interface with non loopback IP will be chosen\n\n"

            "\t\t -pt <port-ranges>\n"
                    "\t\t\t is command for specifying ports for TCP scanning \n\n"

            "\t\t -pt <port-ranges>\n"
                    "\t\t\t is command for specifying ports for UDP scanning\n\n"

            "\t\t <address> \n"
                    "\t\t\t is domain name or IP address of the host, on which the program will be scanning ports \n\n";

    exit(0);
}

/*
 * Main function - parses arguments
 * Creates array of ports
 * Sends them to connectToAddress function
*/
int main(int argc, char *argv[]) {
    vector<string> args(argv+1, argv + argc);
    if (args.size() == 1 && args.at(0) == "-help"){
        printHelp();
    }
    if (args.size() != 7 && args.size() != 5 && args.size() != 3) {
        exitErr("Wrong argument combination", 22);
    }
    int ptCount = 0;
    int puCount = 0;
    int iCount = 0;
    vector<int> portsPT, portsPU, takenPositions;
    ifaddrs *interf;
    string address;


    // cycle through every argument and determine his purpose
    for (int j = 0; j < args.size(); j++) {
        if (j == (args.size() - 1)) break;
        string arg = args.at(j);
        string s = args.at(j + 1);
        takenPositions.push_back(j);
        takenPositions.push_back(j+1);

        if (arg == "-pt"){
            if (ptCount) exitErr("Wrong argument combination", 22);
            portsPT = extractPorts(s);
            ptCount++;
        }
        else if (arg == "-pu"){
            if (puCount) exitErr("Wrong argument combination", 22);
            portsPU = extractPorts(s);
            puCount++;
        }
        else if(arg == "-i"){
            if (iCount) exitErr("Wrong argument combination", 22);
            interf = assignInterf(s);
            iCount++;
        }
        else{
            takenPositions.pop_back();
            takenPositions.pop_back();
        }
    }

    // control check for valid arguments input
    // throwing away wrong arguments combinations
    if (ptCount == 0 && puCount == 0) exitErr("Wrong argument combination", 22);
    if (((args.size() == 5 || args.size() == 7) &&
        (
             (ptCount == 1 && puCount == 0 && iCount == 0) ||
             (ptCount == 0 && puCount == 1 && iCount == 0) ||
             (ptCount == 0 && puCount == 0 && iCount == 1)

        ))) exitErr("Using not allowed arguments", 17);
    if (args.size() == 7){
        if ((ptCount == 1 && puCount == 1 && iCount == 0) ||
            (ptCount == 1 && puCount == 0 && iCount == 1) ||
            (ptCount == 0 && puCount == 1 && iCount == 1))
            exitErr("Using not allowed arguments", 17);
    }

    // assign address string and given interface
    address = args.at(getAddrPos(args.size(), takenPositions));
    if(iCount == 0) interf = getFirstNonLoopbackI();

    // lets begin
    connectToAddress(portsPT, portsPU, interf, address);
}
