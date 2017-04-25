from __future__ import print_function
from scapy.all import *
import re



def main():
    pat = re.compile('(\w*)=([a-zA-Z0-9.]*)')

    print('--- Constructing IP Header ---')
    # Construct IP Header
    ip = open('ip.txt', 'r')
    ip_header = IP()
    for line in ip:
        match = pat.match(line)
        attr = match.group(1)
        val = match.group(2)
        if val.isdigit():
            val = int(val)
        ip_header.setfieldval(attr, val)

    ip.close()

    print('--- Constructing TCP Header ---')
    # Construct TCP Header
    tcp = open('tcp.txt', 'r')
    tcp_header = TCP()
    for line in tcp:
        match = pat.match(line)
        attr = match.group(1)
        val = match.group(2)
        if val.isdigit():
            val = int(val)
        tcp_header.setfieldval(attr, val)

    tcp.close()

    # Send TCP Packet
    tcp_packet = ip_header / tcp_header / Raw(b'This is a TCP Packet!')
    tcp_packet.show()
    print('--- Sending TCP Packet ---')
    send(tcp_packet)

    print('--- Constructing UDP Header ---')
    # Construct UDP Packet
    udp = open('udp.txt', 'r')
    udp_header = UDP()
    for line in udp:
        match = pat.match(line)
        attr = match.group(1)
        val = match.group(2)
        if val.isdigit():
            val = int(val)
        udp_header.setfieldval(attr, val)

    udp_packet = ip_header / udp_header / Raw(b'This is a UDP Packet!')
    udp_packet.show()
    print('--- Sending UDP Packet ---')
    send(udp_packet)


if __name__ == '__main__':
    main()

