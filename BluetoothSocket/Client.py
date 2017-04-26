import bluetooth # From module pybluez

serverMACAddr = 'A4:34:D9:62:71:58' # Server's bluetooth MAC Address
port = 1    # Port to use

# devices = bluetooth.discover_devices()
# print(devices)

# Create a socket and connect
s = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
s.connect((serverMACAddr, port))

while True:
    ch = input('''
1. Receive String
2. Receive File
3. Exit
Enter your choice: ''')

    s.send(str(ch))

    if ch == 1:
        data = s.recv(1024)
        print('Received string is: {}'.format(data))
    elif ch == 2:
        flist = s.recv(1024) # Receive file list
        flist = flist.split('\n') # Convert string to list()
        for i, name in enumerate(flist): # Display choice to user
            print('{}:  {}'.format(i, name))
        fch = input('Enter your choice: ')
        s.send(str(fch)) # Send choice of file to server
        fdata = s.recv(1024) # Receive file data
        # Create and write file
        file = open(flist[int(fch)], 'w')
        file.write(fdata)
        file.close()
        print('Data written to file: {}'.format(flist[int(fch)]))
        print('Data is: \n{}'.format(fdata))
    elif ch == 3:
        break
    else:
        print('Invalid Choice')