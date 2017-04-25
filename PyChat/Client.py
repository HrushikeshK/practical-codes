import socket
import select
import sys
from Colors import Colors
import os

stdin = os.open('/dev/fd/0', os.O_RDWR)


HOST = socket.gethostbyname('localhost')
PORT = 9091

s = socket.socket()
s.connect((HOST, PORT))

username = input('Enter username: ')
s.send(str.encode(username))

prev_msg = ''

while True:
    rdlist, wrlist, errlist = select.select([sys.stdin, s], [], [])
    if sys.stdin in rdlist:
        data = sys.stdin.readline()
        data1 = str.encode(data)
        s.send(data1)
        if '!exit' in data:
            break
    if s in rdlist:
        os.write(stdin, b'\n')
        prev_msg = sys.stdin.readline()
        if len(prev_msg) == 0:
            sys.stdin.buffer.raw.flush()
        incoming = bytes.decode(s.recv(100))
        sys.stdout.write('\r')
        sys.stdout.flush()
        print(incoming)
        if len(incoming) == 0:
            break
    print(Colors.OKBLUE + '\nYou> ' + Colors.ENDC + prev_msg, end='')
    prev_msg = ''

s.shutdown(socket.SHUT_RDWR)
s.close()
