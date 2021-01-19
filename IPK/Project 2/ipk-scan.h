#ifndef IPKSCAN_H
#define IPKSCAN_H

#include <iostream>
#include <vector>
#include <regex>		// regex..duh
#include <pcap.h>		// filter
#include <unistd.h>		// alarm
#include <signal.h>		// alarm
#include <netdb.h>          	// hostent -- gethostbyname()
#include <net/if.h>         	// ifreq && loopback check
#include <arpa/inet.h>      	// convert ip in print
#include <ifaddrs.h>        	// sin
#include <netinet/ip.h>		// ip header
#include <netinet/tcp.h>	// tcp header
#include <netinet/if_ether.h>   // ethernet struct
#include <netinet/udp.h>	// udp header

using namespace std;

struct pseudo_header{
    u_int32_t source_address;
    u_int32_t dest_address;
    u_int8_t zero;
    u_int8_t protocol;
    u_int16_t tcp_length;
};

void exitErr();
int getAddrPos();
vector<int> makeArrayFromString();
vector<int> extractPorts();
ifaddrs *getAllInterfaces();
ifaddrs *getFirstNonLoopbackI();
ifaddrs *assignInterf();
void printGiven();
uint16_t checkSum();
void printState();
void processICMP();
void processTCP();
void handlePacket();
void createHandler();
iphdr fillIPHeaderTCP();
tcphdr fillTCPHeader();
sockaddr_in fillSin();
pseudo_header fillPseudoHeader();
void connectViaTCP();
iphdr fillIPHeaderUDP();
void connectViaUDP();
void connectToAddress();

#endif
