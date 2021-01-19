/*===================================================*
 *|                                                 |*
 *|     Author:     Martin Macháček (xmacha73)      |*
 *|     Project:    ISA                             |*
 *|                 Varianta 4                      |*
 *|                 Whois tazatel                   |*
 *|                                                 |*
 *|     Date:       18.11.2019                      |*
 *|                                                 |*
 *===================================================*/

#include <unistd.h>
#include <iostream>
#include <arpa/inet.h>
#include <netdb.h>
#include <cstring>
#include <vector>
#include <resolv.h>
#include <algorithm>
#include <arpa/nameser.h>

using namespace std;

#define ERR_ARGS 100
#define ERR_CONNECT 101
#define ERR_SEND 102
#define ERR_REALLOC 103
#define ERR_INVALID_DOM 104

bool user_unmodified_whois = false;
bool user_only_one_ip = false;
string domain_message;

/*
 *  Structure for for domain information
 *  ip          ip address
 *  is_ipv6     boolean value to determine ip version
 */
struct iaf{
    char *ip;
    bool is_ipv6;
};

/*
 *  Prints out arguments from user
 *  *only for testing purposes*
 */
void print_args(string* args){
    cout << "------------------------------\n";
    cout << "|   -q:  " << args[0] << "\n";
    cout << "|   -w:  " << args[1] << "\n";
    cout << "|   -d:  " << args[2] << "\n";
    cout << "------------------------------\n";
}

/*
 *  Prints elements of given string vector and
 *  uses given string as a separator for those elements.
 *  vector<string> d    - string vector for printing
 *  string delim        - string, which will be used to separate printed elements
 */
void print_data(vector<string> d, string delim){
    for (int i = 0; i < d.size(); i++){
        if (!(d.at(i).empty())){
            cout << d.at(i) << delim;
        }
    }
}

/*
 * Prints help to the user
 */
void print_help(){
    cout <<
         "============================= HELP =============================\n\n"
         "This program does WHOIS & DNS lookup\n"
         "WHOIS lookup searches for all available data about the address from given whois server,\n"
         "and then it deletes the 'not needed' information.\n"
         "DNS lookup takes the input exactly as the user inputs it and does a dns query for given domain.\n"

         "+ Added -f switch\n"
         "+ Added -o switch\n\n"

         "This program doesnt remove 'www' from given address, it realies on the users knowledge of used \n"
         "whois servers and its rules & ways of giving information.\n\n"

         "Program has 5 arguments/options/switches:\n"
         "\t-q <IP|hostname>\t (compulsory)\n"
         "\t\t<IP|hostname> =\tIP address or hostname of desired domain address\n"
         "\t\tf.e.\t-q seznam.cz\n"
         "\t\t\t-q www.google.com\n"
         "\t\t\t-q 77.75.75.172\n\n"
         "\t-w <IP|hostname>\t (compulsory)\n"
         "\t\t<IP|hostname> =\tIP address or hostname of desired whois server\n"
         "\t\tf.e.\t-w whois.ripe.net\n"
         "\t\t\t-w whois.arin.net\n"
         "\t\t\t-w whois.iana.org\n\n"
         "\t-d <IP>\t\t\t(optional)\n"
         "\t\t<IP> =\tIP address of desired DNS for DNS Lookup, IT DOESNT WORK WITH IPv6!!!\n"
         "\t\tf.e.\t-d 8.8.8.8\n\n"
         "\t-f\t\t\t(optional)\n"
         "\t\tIf user wants unmodified whois information, then this switch will\n"
         "\t\tdisable the output modifier and user will get clean, unmodified server response.\n\n"
         "\t-o\t\t\t(optional)\n"
         "\t\tIf user wants to get whois information only from the first IP address\n"
         "\t\ton given -q domain.\n\n"

         "These arguments do not have special order rules.\n"
         "Final run command can look f.e. like this:\n"
         "\t ./isa-tazatel -w whois.ripe.net -q seznam.cz -o\n"
         "\t ./isa-tazatel -w 193.0.6.135 -q www.google.com -f\n"
         "\t ./isa-tazatel -w whois.ripe.net -q 2a02:598:4444:1::1\n\n";
}

/*
 *  Parses input arguments given by user.
 *  If user gives input that isnt supposed to be there, program will tell him, and will stop.
 *  char *arguments[]   - char array full of arguments from user
 *  int size            - number of arguments given by user
 *  return              - array of strings (size 2), each string is successfully parsed argument from user
 */
string* parse_args(char *arguments[], int size){
    if (size == 2){
        if (strcmp(arguments[1], "--help") == 0 || strcmp(arguments[1], "-help") == 0){
            print_help();
            exit(0);
        }
    }
    if (size < 5) return NULL;
    int option;
    int qflag = 0;
    int dflag = 0;
    int wflag = 0;
    int fflag = 0;
    int oflag = 0;

    string* values = new string[3];

    while (optind < size){
        if((option = getopt(size, arguments, "q:w:d:fo")) != -1){
            switch (option){
                case 'q':
                    if (qflag != 0){
                        return NULL;
                    }
                    qflag++;
                    values[0] = optarg;
                    break;
                case 'w':
                    if (wflag != 0){
                        return NULL;
                    }
                    wflag++;
                    values[1] = optarg;
                    break;
                case 'f':
                    if (fflag != 0){
                        return NULL;
                    }
                    fflag++;
                    user_unmodified_whois = true;
                    break;
                case 'o':
                    if (oflag != 0){
                        return NULL;
                    }
                    oflag++;
                    user_only_one_ip = true;
                    break;
                case 'd':
                    if (dflag != 0){
                        return NULL;
                    }
                    dflag++;
                    values[2] = optarg;
                    break;
                case '?':
                    return NULL;
            }
        }
        else{
            return NULL;
        }
    }
    if (wflag == 0 || qflag == 0){
        cout << "Missing -w flag or -q flag\n";
        return NULL;
    }
    return values;
}

/*
 *  Checks if given string is in domain address format or an IP format. If its an IP format,
 *  the function checks the validity of it and the function returns the result accordingly
 *  string address      - given string, it's format will be checked
 *  return              - true/false
 */
bool is_correct_ip_format(string address){
    for (char c : address){
        if (!isdigit(c) && c != '.'){
            return true;
        }
    }

    struct sockaddr_in sa;
    if(inet_pton(AF_INET, (char*)address.c_str(), &(sa.sin_addr)) == 1){
        return true;
    }
    return false;
}

/*
 *  Initializes given socket
 *  int *s       - socket to be initialized
 */
void new_socket(int *s){
    *s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    ////cout << "Vytvoril sem ipv4 socket\n";
}

/*
 * Initializes given socket for ipv6
 * int *s       - socket to be initialized for ipv6
 */
void new_socket6(int *s){
    *s = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
    ////cout << "Vytvoril sem ipv6 socket\n";
}

/*
 *  Prepares sockaddr_in structure
 *  sets flag for ipv4 family, fills destination ip and destination port (43)
 *  struct sockaddr_in *d       - structure into which the function whill fill needed information
 *  char ip[32]                 - ip address of destination domain
 */
void fill_address(struct sockaddr_in *d, char ip[32]){
    (*d).sin_family = AF_INET;
    (*d).sin_addr.s_addr = inet_addr(ip);
    (*d).sin_port = htons(43);
}

/*
 *  Prepares sockaddr_in6 structure
 *  sets flag for ipv6 family, fills destination ip and destination port (43)
 *  struct sockaddr_in6 *d      - structure into which the function whill fill needed information
 *  char ip[46]                 - ip address of destination domain
 */
void fill_address6(struct sockaddr_in6 *d, char ip[46]){
    (*d).sin6_family = AF_INET6;
    inet_pton(AF_INET6, ip, &d->sin6_addr);
    (*d).sin6_port = htons(43);
}

/*
 *  Checks if vector of iaf structures already contains given ip address
 *  vector<iaf> *v      - vector of iaf structures containing ip, and bool value about ipv6
 *  char *ip            - ip address, that the function will be looking for in the vector
 *  return              - true/false
 */
bool is_ip_already_saved(vector<iaf> *v, char* ip){
    for (int i = 0; i < v->size(); i++){
        if (user_only_one_ip){
            return true;
        }
        else if(strcmp(ip, v->at(i).ip) == 0){
            return true;
        }
    }
    return false;
}

/*
 *  Creates ip structures from given domain address. First, it checks for validity of the address,
 *  by doing this, it gets structure of information about the given domain, and then through cycling, it
 *  collects all the needed information and adds it into the vector of iaf structures.
 *  vector<iaf> *domain_ips     - vector of iaf structures, to which the the function will push information
 *  char* name                  - domain name, from which all the data will be fetched
 */
void get_ip_structs(vector<iaf> *domain_ips, char* name){
    struct addrinfo h, *p;
    struct addrinfo *addr_info;
    memset(&h, 0, sizeof h);
    iaf *x = (iaf*)malloc(sizeof(iaf));

    if (getaddrinfo(name, NULL, &h, &addr_info) != 0) {
        cout << "Address "<< name <<" doesnt seem to be valid.\n";
        exit(ERR_INVALID_DOM);
    }
    for (p = addr_info; p != NULL; p = p->ai_next) {
        struct in_addr  *addr;
        memset(x, 0, sizeof(iaf));

        if (p->ai_family == AF_INET) {
            x->ip = (char*)malloc(INET_ADDRSTRLEN);
            struct sockaddr_in *ipv = (struct sockaddr_in *)p->ai_addr;
            addr = &(ipv->sin_addr);
            inet_ntop(p->ai_family, addr, x->ip, INET_ADDRSTRLEN);
            x->is_ipv6 = false;
        }
        else if(p->ai_family == AF_INET6){
            x->ip= (char*)malloc(INET6_ADDRSTRLEN);
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            addr = (struct in_addr *) &(ipv6->sin6_addr);
            inet_ntop(p->ai_family, addr, x->ip, INET6_ADDRSTRLEN);
            x->is_ipv6 = true;
        }
        else{
            cout << "Address "<< name <<" doesnt seem to be valid.\n";
            exit(ERR_INVALID_DOM);
        }

        if (is_ip_already_saved(domain_ips, x->ip) == true) continue;
        (*domain_ips).push_back(*x);
    }
    freeaddrinfo(addr_info);
    return;
}

/*
 *  The collecting & assigning result to char **res was completed thanks to:
 *      https://stackoverflow.com/questions/4840535/c-recv-problem  -   Answer from jqweyrich
 *  Sends filled socket and collects data from response from the server
 *  iaf domain      - domain address, which is needed for message content
 *  int s           - socket via which we will be sending the message
 *  char **res      - result/response from the given address
 */
void send_and_collect(iaf domain, int s, char **res){
    char content[100];
    int r_size = 0;
    int t_size = 0;
    char buffer[1500];

    if (!(domain_message.empty())){
        sprintf(content , "%s\r\n", (char*)domain_message.c_str());
        domain_message.clear();
    }
    else{
        sprintf(content , "%s\r\n", domain.ip);
    }


    if (send(s, content, strlen(content), 0) < 0){
        cout << "Failed to send content to server\n";
        exit(ERR_SEND);
    }
    while((r_size = recv(s, buffer, sizeof(buffer), 0))){
        *res = (char*)realloc(*res, t_size + r_size);
        if (*res == NULL){
            cout << "Failed to find any data\n";
            exit(ERR_REALLOC);
        }
        memcpy(*res + t_size, buffer, r_size);
        t_size += r_size;
    }
    *res = (char*)realloc(*res , t_size + 1);
    *(*res + t_size) = '\0';
}

/*
 *  Checks if given string contains strings "Allocated to" or "NON-RIPE-"
 *  string res      - string in which function will be searching
 *  return          - true/false
 */
bool is_diff_whois(string res){
    string arin_to_diff = "Allocated to ";
    string ripe_to_diff = "NON-RIPE-";

    if (res.find(arin_to_diff) != string::npos ||
        res.find(ripe_to_diff) != string::npos){
        return true;
    }
    return false;
}

/*
 *  Checks if given string (char*) falls into allowed strings.
 *  char* s     - string to be compared with allowed strings
 *  return      - true/false
 */
bool is_allowed(char* s){
    const int count = 58;
    char list[count][50]={
            "Updated:",
            "OrgAbuse",
            "OriginAS",
            "OrgNOCHandle",
            "OrgNOCName",
            "OrgNOCPhone",
            "OrgNOCEmail",
            "OrgNOCRef",
            "RTechHandle",
            "RTechName",
            "RTechPhone",
            "RTechEmail",
            "RTechRef",
            "RAbuseHandle",
            "RAbuseName",
            "RAbusePhone",
            "RAbuseRef",
            "RAbuseEmail",
            "RNOCHandle",
            "RNOCPhone",
            "RNOCEmail",
            "RNOCRef",
            "RNOCName",
            "Comment",
            "Parent:",
            "OrgTech",
            "Ref",
            "ResourceL",
            "RegDate:",
            "remarks:",
            "last-mod",
            "nic-",
            "created:",
            "origin:",
            "abuse",
            "status:",
            "mnt-ref",
            "mnt-routes",
            "mnt-domains",
            "mnt-lower",
            "assignment-size",
            "org-type",
            "route",
            "role",
            "tech-c",
            "source:",
            "nsstat:",
            "nslastaa:",
            "changed:",
            "inetrev:",
            "ownerid:",
            "mnt-by:",
            "keyset",
            "nsset",
            "dnskey",
            "registrar",
            "ds-rdata",
            "nserver"
    };

    for (int i = 0; i < count; i++){
        if (strncmp(s, list[i], strlen(list[i])) == 0){
            return false;
        }
    }
    return true;
}

/*
 *  Parts of this function were inspired by second IPK project - OMEGA
 *  Main function for WHOIS lookup. It takes arguments and uses functions above to create ip
 *  structures, fill addresses and structures with needed data, and uses functions to send and collect data
 *  from given whois server about given address.
 *  Finally, it parses the output and adds it into the vector of vectors of strings.
 *  string* arguments               - parsed arguments (whois, domain addresses)
 *  vector<vector<string>> *data    - vector of vectors of strings, to which the retrieved data will be pushed
 */
void get_whois(string* arguments, vector<vector<string>> *data){
    res_init();
    char *domain_address = (char*)(arguments[0].c_str());
    char *whois_address = (char*)(arguments[1].c_str());

    struct timeval timeout;
    timeout.tv_sec = 4;
    timeout.tv_usec = 0;
    int s = 0;
    int connect_res;
    char *res;
    vector<string> f_result;
    struct sockaddr_in d;
    struct sockaddr_in6 d6;
    vector<iaf> whois_ips, domain_ips;
    res = (char*)malloc(50);
    struct in6_addr test_ip6;
    struct in_addr test_ip;

    if ((inet_pton(AF_INET, domain_address, &test_ip) != 1) &&
        (inet_pton(AF_INET6, domain_address, &test_ip6) != 1)){

        domain_message = domain_address;
        domain_ips.push_back({domain_address, false});
    }
    
    get_ip_structs(&domain_ips, domain_address);
    get_ip_structs(&whois_ips, whois_address);

    for (int i = 0; i < domain_ips.size(); i++){

        f_result.clear();
        if (whois_ips.at(0).is_ipv6 == false){
            new_socket(&s);
            memset(&d, 0, sizeof(d));
            fill_address(&d, whois_ips.at(0).ip);
        }
        else{
            new_socket6(&s);
            memset(&d6, 0, sizeof(d6));
            fill_address6(&d6, whois_ips.at(0).ip);
        }

        setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));
        setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout));

        whois_ips.at(0).is_ipv6 == true ?
                connect_res = connect(s, (const struct sockaddr*) &d6, sizeof(d6)) :
                connect_res = connect(s, (const struct sockaddr*) &d, sizeof(d));

        if (connect_res < 0){
            cout << "Connect failed - probably given whois server "<< whois_address<< " doesnt respond/doesnt exist. \n"
                    "If youre trying to get information from IPv6 whois, then you must have IPv6 yourself.\n";
            exit(ERR_CONNECT);
        }

        send_and_collect(domain_ips.at(i), s, &res);
        close(s);
        res = strtok(res, "\n");

        (*data).push_back({"\nQuerying for:",domain_ips.at(i).ip,"--------------------------"});

        while(res != NULL){
            if (user_unmodified_whois){
                f_result.push_back(res);
            }
            else{
                if((res[0] != '%')             &&
                   (res[0] != '#')             &&
                   (strcmp(res, "\r") != 0)    &&
                   (is_allowed(res))){
                    if (strncmp(res, "NetType:", 8)  == 0 ||
                        strncmp(res, "netname:", 8)  == 0){
                        if (is_diff_whois(res)){
                            (*data).push_back({"This address is not managed by chosen whois, but some information is available.\n"});
                        }
                    }
                    f_result.push_back(res);
                }
            }
            res = strtok(NULL , "\n");
        }
        if (f_result.empty()){
            f_result.push_back({"No information was received from given whois server.\n"});
        }
        (*data).push_back(f_result);
    }
}

/*
 *  Removes duplicates inside vector of strings.
 *  Leaves the duplicates as blank elements
 *  vector<string> **data       - vector of strings from which the function will erase duplicate elements
 */
void remove_vstr_duplicates(vector<string> **data){
    for (int i = 0; i < (*data)->size(); i++){
        for(int y = i+1; y < (*data)->size(); y++){
            if ((*data)->at(i) == (*data)->at(y)){
                (*data)->at(i).erase();
            }
        }
    }
}

/*
 *  Makes vector out of string by given 'delimiter', which will
 *  determine what will separate these elements
 *  string delim        - delimiter, which will be the separator of vector elements
 *  string *s           - strings from which the vector will be made
 *  return              - vector of strings
 */
vector<string> vectorize_string(string delim, string *s){
    vector<string>result;
    while(s->size()){
        int i = s->find(delim);
        if(i != string::npos){
            result.push_back(s->substr(0, i));
            *s = s->substr(i + delim.size());
            if (s->size() == 0){
                result.push_back(*s);
            }
        }
        else{
            result.push_back(*s);
            *s = "";
        }
    }
    return result;
}

/*
 *  Parses/Edits given string of data. The data string is in format given by server response.
 *  First, the function checks for \t signs, and replaces them with spaces, then, it erases duplicate spaces
 *  and then the function vectorizes the string using space, and after deciding what record is it formatting,
 *  it collects the needed records and puts them together, then returns the final edited string.
 *  string data     - data, which will be edited/formatted
 *  return          - formated/parsed/edited string
 */
string parse_dns_info(string data){
    replace(data.begin(), data.end(), '\t', ' ');

    for (int i = 0; i < data.length(); i++){
        if (data[i] == ' ' && data[i+1] == ' '){
            data.erase(i,1);
        }
        if (data[i] == '\n'){
            data.erase(i);
        }
    }

    vector<string> out;
    out = vectorize_string(" ", &data);

    //print_data(out, "\n");

    string record = out.at(3);
    string final_output;

    if (record == "A" || record == "AAAA" || record == "NS" || record == "CNAME" || record == "PTR"){
        out.at(4).insert(0, ":\t\t");
        out.at(4).insert(0, record);
        final_output = out.at(4);
    }
    if (record == "MX"){
        out.at(5).insert(0, " - ");
        out.at(5).insert(0, out.at(4));
        out.at(5).insert(0, ":\t\t");
        out.at(5).insert(0, record);
        final_output = out.at(5);
    }
    if (record == "SOA"){
        out.at(5).insert(0, ":\t");
        out.at(5).insert(0, "\nAdmin-email");
        out.at(4).insert(0, ":\t\t");
        out.at(4).insert(0, record);
        int i = out.at(5).find_first_of(".");
        out.at(5)[i]='@';
        out.at(5).insert(0, out.at(4));
        final_output = out.at(5);
    }
    return final_output;
}

/*
 *  Makes a query to given domain, with given record type, collects the response data
 *  parses them and pushes the final data info vector of string.
 *  int info                - record type for res_query.
 *  char* domain            - domain address to make query to
 *  vector<string> **data   - vector of string elements, containing information from completed query.
 */
void query_info_dns(int info, char* domain, vector<string> **data){
    vector<string> response;
    u_char answer[1024];
    ns_msg msg;
    ns_rr rr;
    char dispbuf[4096];
    string x;

    int y;
    if ((y = res_query(domain, ns_c_in, info, answer, sizeof(answer))) < 0){}

    else{
        ns_initparse(answer, y, &msg);
        y = ns_msg_count(msg, ns_s_an);

        for (int i = 0; i < y; i++){
            ns_parserr(&msg, ns_s_an, i, &rr);
            ns_sprintrr(&msg, &rr, NULL, NULL, dispbuf, sizeof(dispbuf));
            x = dispbuf;
            x = parse_dns_info(x);
            (*data)->push_back(x);
        }
    }
}

/*
 *  Removes all occurences of character from string, by comparing each character of string to given char
 *  string str      - string from which the function will remove all occurences of given char
 *  char target     - character that will be deleted from string
 *  return          - string that doesnt contain given char anymore
 */
string remove_char_from_string(string str, char target){
    string result;
    for (char c : str){
        if (c != target){
            result += c;
        }
    }
    return result;
}

/*
 *  Expanding of ipv6 was inspired by
 *  https://stackoverflow.com/questions/3727421/expand-an-ipv6-address-so-i-can-print-it-to-stdout - answer from nategoose
 *  Expands given ipv6 address and then reverses it to make it suitable for reverse ipv6 res_query ptr record.
 *  Thanks to sprintf, it is able to fill in the empty octets, and then, using reverse() and insert in cycle, it
 *  is able to reverse the ip address and convert it to needed format.
 *  string address      - string holding ipv6 address
 *  return              - expanded and formated ipv6 address, ready to be put into res_query
 */
string expand_and_reverse_ipv6(string address){
    struct in6_addr ipv6;
    char str[40];
    string long_ipv6;

    if(inet_pton(AF_INET6, (char*)address.c_str(), &ipv6)) {
        sprintf(str, "%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
                (int) ipv6.s6_addr[0], (int) ipv6.s6_addr[1],
                (int) ipv6.s6_addr[2], (int) ipv6.s6_addr[3],
                (int) ipv6.s6_addr[4], (int) ipv6.s6_addr[5],
                (int) ipv6.s6_addr[6], (int) ipv6.s6_addr[7],
                (int) ipv6.s6_addr[8], (int) ipv6.s6_addr[9],
                (int) ipv6.s6_addr[10], (int) ipv6.s6_addr[11],
                (int) ipv6.s6_addr[12], (int) ipv6.s6_addr[13],
                (int) ipv6.s6_addr[14], (int) ipv6.s6_addr[15]);

        long_ipv6 = str;
        reverse(long_ipv6.begin(), long_ipv6.end());
        long_ipv6 = remove_char_from_string(long_ipv6, ':');
        for (int i = 1; i < 32; i++){
            long_ipv6.insert(32-i, ".");
        }
    }
    long_ipv6.insert(long_ipv6.size(), ".ip6.arpa.");
    return long_ipv6;
}

/*
 *  Reverses given ipv4, to make it suitable for res_query reverse dns record lookup.
 *  First, it vectorizes the given address, and then it reverses the octets + formats the ip
 *  to a suitable format
 *  char* domain_address        - ipv4 address to be reversed and formatted
 *  return                      - reversed, formatted ipv4
 */
char* get_reverse_ipv4(char* domain_address){
    string s = domain_address;
    vector<string> result = vectorize_string(".", &s);
    reverse(result.begin(), result.end());
    for (const auto &block : result) s += block+".";
    s.pop_back();
    s.insert(s.size(), ".in-addr.arpa");
    char *final_res = new char[s.length() + 1];
    strcpy(final_res, s.c_str());

    return final_res;
}

/*
 *  Main function for DNS lookup. First, it check validity of domain address given by user.
 *  Ff it is ip address, function makes reverse dns query for ptr record.
 *  If it is not an ip, it skips the reverse ptr, and just queries the domain address by with given record type.
 *  string* arguments           - parsed arguments (whois, domain addresses)
 *  vector<string> *data        - vector of strings, to which the retrieved data will be pushed
 *  vector<string> *missing     - vector of strings, that will contain missing DNS records
 */
void get_dns(string* arguments, vector<string> *data, vector<string> *missing){
    string domain_address = (char*)(arguments[0].c_str());

    struct addrinfo h;
    struct addrinfo *addr_info;
    struct sockaddr_in sa;
    struct sockaddr_in6 sa6;
    char str[INET_ADDRSTRLEN];

    res_init();
    _res.nscount = 1;
    _res.retrans = 1;

    if (!arguments[2].empty()){
        if (inet_pton(AF_INET, (char*)arguments[2].c_str(), &(_res.nsaddr_list[0].sin_addr)) != 1){
            cout << "Given DNS: '"<<(char*)arguments[2].c_str()<<"' doesnt exist or the format isnt valid.\n"
                    "Program will use default OS DNS\n";
        }
    }
    inet_ntop(AF_INET, &(_res.nsaddr_list[0].sin_addr), str, INET_ADDRSTRLEN);
    cout << "Using DNS: " <<  str <<"\n\n";

    memset(&h, 0, sizeof h);
    if (getaddrinfo((char*)domain_address.c_str(), NULL, &h, &addr_info) != 0) {
        cout << "Address "<< domain_address <<" doesnt seem to be valid / Used DNS didnt find it.\n";
        return;
    }


    if (inet_pton(AF_INET, (char*)domain_address.c_str(), &(sa.sin_addr)) == 1){
        char *reverse_ip;
        reverse_ip = get_reverse_ipv4((char*)domain_address.c_str());
        query_info_dns(ns_t_ptr, reverse_ip, &data);
    }
    else if(inet_pton(AF_INET6, (char*)domain_address.c_str(), &(sa6.sin6_addr)) == 1){
        query_info_dns(ns_t_ptr, (char*)expand_and_reverse_ipv6(domain_address).c_str(), &data);
    }
    if ((*data).size() > 0){
        string x = (*data).at(0);
        domain_address = x.substr(x.find("\t") + 2);
    }

    query_info_dns(ns_t_a, (char*)domain_address.c_str(), &data);
    query_info_dns(ns_t_aaaa, (char*)domain_address.c_str(), &data);
    query_info_dns(ns_t_mx, (char*)domain_address.c_str(), &data);
    query_info_dns(ns_t_cname, (char*)domain_address.c_str(), &data);
    query_info_dns(ns_t_ns, (char*)domain_address.c_str(), &data);
    query_info_dns(ns_t_soa, (char*)domain_address.c_str(), &data);

    remove_vstr_duplicates(&data);

    vector<string> records;
    vector<string> needed_records;

    needed_records.push_back("A");
    needed_records.push_back("AAAA");
    needed_records.push_back("MX");
    needed_records.push_back("CNAME");
    needed_records.push_back("NS");
    needed_records.push_back("SOA");
    needed_records.push_back("PTR");

    for (int i = 0; i < data->size(); i++){
        string::size_type pos = data->at(i).find(':');
        if (pos != string::npos){
            records.push_back(data->at(i).substr(0, pos));
        }
    }

    for (int i = 0; i < records.size(); i++){
        for (int y = 0; y < needed_records.size(); y++){
            if (needed_records.at(y) == records.at(i)){
                needed_records.at(y).erase();
            }
        }
    }
    (*missing) = needed_records;
}

/* -----------------------------------------
 * -----------------------------------------
 * Main function - calls WHOIS & DNS lookup
 * Prints retrieved data after each function
 * -----------------------------------------
 * -----------------------------------------
 */
int main(int argc , char *argv[]) {
    string* parsed_arguments;
    vector<string> dnsdata;
    vector<vector<string>> whoisdata;
    vector<string> missing;

    if ((parsed_arguments = parse_args(argv, argc)) == NULL){
        cout << "Wrong arguments input! use -help for more info.\n";
        exit(ERR_ARGS);
    }
    if (!is_correct_ip_format(parsed_arguments[0]) ||
        !is_correct_ip_format(parsed_arguments[1])){
        cout << "Wrong arguments input! use -help for more info.\n";
        exit(ERR_ARGS);
    }

    cout << "\n========================  DNS  ========================\n\n";
    get_dns(parsed_arguments, &dnsdata, &missing);
    for (int i = 0; i < dnsdata.size(); i++){
        if (dnsdata.at(i).size() != 0){
            cout << dnsdata.at(i)<< endl;
        }
    }
    if (!missing.empty()){
        cout<< "Missing records: \n";
        print_data(missing, "   ");
        cout<<"\n";
    }

    cout << "\n=======================  WHOIS  =======================\n";
    get_whois(parsed_arguments, &whoisdata);
    for (int i = 0; i < whoisdata.size(); i++){
        print_data(whoisdata.at(i), "\n");
    }

    return 0;
}
