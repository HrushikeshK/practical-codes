from __future__ import print_function
import socket
import random

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM) 
host = socket.gethostname()                           
port = 9999
s.connect((host, port))                               

while True:
        choice = int(raw_input("\n1. Enter 1 for Go Back N Protocol\n2. Enter 2 for Selective Repeat Protocol\n3. Enter 3 to exit\nEnter your choice : "))
        s.send(str(choice))
        if(choice == 3 or (choice!=1 and choice!=2)):
                break
        
        window_size = raw_input("\nEnter the window size : ")
        s.send(window_size)
        
        message = []
        print("Receiving frames")
        
        # Receive Message Length
        len_msg = int(s.recv(10))
        s.send("Message Length is received")
        for i in range(len_msg):
                a = s.recv(1)
                print("Frame received : ", a)
                message.extend(a)
        
        #Randomly drop a frame 
        random_frame_no = int(random.random() * len_msg)
        if(choice == 1):
                message = message[:random_frame_no]
        elif(choice==2):
                message[random_frame_no] = "<NACK_SENT>"
        print("Message received till now : ", "".join(message))
        print("Sending NACK randomly for frame no. : ",random_frame_no+1)
        s.send(str(random_frame_no))
        
        if(choice == 1):
                for i in range(len_msg - random_frame_no):
                        a = s.recv(1)
                        print("Frame received : ", a)
                        message.extend(a)
        elif(choice==2):
                a = s.recv(1)
                print("Frame received : ", a)
                message[random_frame_no] = a
        
        print("Message finally received : ", "".join(message))
                

s.close()
