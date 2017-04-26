import bluetooth
import os

serverMACAddr = 'A4:34:D9:62:71:58'
port = 1

s = bluetooth.BluetoothSocket(bluetooth.RFCOMM)

s.bind(("", port)) # Bind to '' to accept connections from all addresses on port 1
s.listen(1) # Listen

client, clientInfo = s.accept() # Accept connection from client
print('Client connected: {}'.format(clientInfo))

while True:
    ch = client.recv(1024) # Receive choice of operation from client
    ch = int(ch)

    if ch == 1: # Send string
        data = input('Enter string to send: ')
        client.send(data) # Send user's string to client
    elif ch == 2: # Send file
        ls = os.listdir('server_data/') # List contents of directory
        client.send('\n'.join(ls)) # Send file list to client
        findx = int(client.recv(1024)) # Receive file choice
        fdata = open('server_data/{}'.format(ls[findx]), 'r').read() # Read file data
        client.send(fdata) # Send file data
    elif ch == 3:
        exit()
    else:
        print('Invalid Choice.')