/*
 * Write a program (C/C++) to analyze following packet formats captured
 * through Wireshark for wired network 
 * 1.Ethernet 2.IP 3.TCP 4.UDP
 ********************************
 * Input: ./analyze <file_name.pcap>
 * Output: Details of each packet found in the .pcap file
 ********************************
 */


#include <iostream>
#include <string.h>
#include <pcap.h>
#include <net/ethernet.h>
#include <netinet/ether.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <arpa/inet.h>

using namespace std;

void packet_handler(u_char *userData, const struct pcap_pkthdr* pkthdr, const u_char* packet);

int ip_packet_count = 0;
int tcp_packet_count = 0;
int udp_packet_count = 0;

int main(int argc, char **argv)
{
	pcap_t *descr;
	char errbuf[PCAP_ERRBUF_SIZE];
	const char *file_name = argv[1];

	// open capture file for offline processing
	descr = pcap_open_offline(file_name, errbuf);

	if (descr == NULL) {
		cout << "pcap_open_offline() failed: " << errbuf << endl;
		return 1;
	}

	if (pcap_loop(descr, 0, packet_handler, NULL) < 0) {
		cout << "pcap_loop() failed: " << pcap_geterr(descr);
		return 1;
	}

	cout << "---- Packet capture finished ----" << endl;
	cout << "Total IP packets: " << ip_packet_count << endl;
	cout << "Total TCP packets: " << tcp_packet_count << endl;
	cout << "Total UDP packets: " << udp_packet_count << endl;

	return 0;
}

/* Callback Method */
void packet_handler(u_char *userData, const struct pcap_pkthdr* pkthdr, const u_char* packet)
{
	const struct ether_header *eth_header;
	struct ether_addr host;
	const struct ip *ip_header;
	const struct tcphdr *tcp_header;
	const struct udphdr *udp_header;
	char src_ip[INET_ADDRSTRLEN];
	char dst_ip[INET_ADDRSTRLEN];
	char *ether_dhost;
	u_int src_port, dst_port;
	u_char *data;
	int data_length = 0;
	string data_str = "";
	bool isTCP = true;

	eth_header = (struct ether_header*)packet;

	// Determine if the packet is IP or not
	if (ntohs(eth_header->ether_type) == ETHERTYPE_IP) {
		++ip_packet_count;

		ip_header = (struct ip*)(packet + sizeof(struct ether_header));
		inet_ntop(AF_INET, &(ip_header->ip_src), src_ip, INET_ADDRSTRLEN);
		inet_ntop(AF_INET, &(ip_header->ip_dst), dst_ip, INET_ADDRSTRLEN);
		memcpy(&host, eth_header->ether_dhost, sizeof(host));
		ether_dhost = ether_ntoa(&host);

		if (ip_header->ip_p == IPPROTO_TCP) {		/* TCP */
			isTCP = true;
			++tcp_packet_count;

			tcp_header = (tcphdr*)(packet + sizeof(struct ether_header) + sizeof(struct ip));
			src_port = ntohs(tcp_header->source);
			dst_port = ntohs(tcp_header->dest);
			data = (u_char*)(packet + sizeof(struct ether_header) + sizeof(struct ip) + sizeof(struct tcphdr));
			data_length = pkthdr->len - (sizeof(struct ether_header) + sizeof(struct ip) + sizeof(struct tcphdr));
		
		} else if (ip_header->ip_p == IPPROTO_UDP) {		/* UDP */
			isTCP = false;
			++udp_packet_count;

			udp_header = (udphdr*)(packet + sizeof(struct ether_header) + sizeof(struct ip));
			src_port = ntohs(udp_header->source);
			dst_port = ntohs(udp_header->dest);
			data = (u_char*)(packet + sizeof(struct ether_header) + sizeof(struct ip) + sizeof(struct udphdr));
			data_length = pkthdr->len - (sizeof(struct ether_header) + sizeof(struct ip) + sizeof(struct udphdr));
		}
	}

	// convert non-printable characters, other than carriage return, line feed,
	// or tab into periods when displayed.
	for (int i = 0; i < data_length; i++) {
		if ((data[i] >= 32 && data[i] <= 126) || data[i] == 10 || data[i] == 11 || data[i] == 13) {
			data_str += (char)data[i];
	  	} else {
		  data_str += ".";
	  	}
	}	

	// Print results
	cout << endl << "MAC Address: " << ether_dhost << endl;
	cout << "Protocol: " << ((isTCP) ? "TCP" : "UDP") << "/IP" << endl;
	cout << "Source IP: " << src_ip << endl;
	cout << "Source Port: " << src_port << endl;
	cout << "Dest IP: " << dst_ip << endl;
	cout << "Dest Port: " << dst_port << endl;
	cout << "Data length: " << data_length << endl;
	cout << "DATA: " << endl;

	if (data_length > 0) 
		cout << data_str << endl;
	cout << endl;
}