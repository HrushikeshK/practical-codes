from __future__ import print_function
from socket import *
import time

#Creating server socket
s = socket(AF_INET, SOCK_STREAM)
s.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)

host = gethostname()                           
port = 9999

#Binding local host and port no. to socket. Change the host ip if you want to run server the client on different machines. Currently set to loopback address
s.bind((host, port))                                  

s.listen(5)                                   

print("Server has started")
#Accept a client requesting for connection
c,addr = s.accept()
print("Connection established with : ",c.getsockname())

#Function to send the message initially
def sending_message(message):
        split_message = [message[i:i+window_size] for i in range(0, len(message), window_size)]
        
        #Send message length
        c.send(str(len(message)))
        print(c.recv(50))
        
        for msg in split_message:
                #Send message character by character
                msg_split_frames = [msg[i:i+1] for i in range(0, len(msg), 1)]
                for fm in msg_split_frames:
                        msg_frames.extend(fm)
                        c.send(fm)

while True:
        msg_frames = []
        choice = int(c.recv(100))
        if(choice==3):
                print("Client {} is exiting ".format(c.getsockname()))
                c.close()
                break
        elif(choice!=1 and choice!=2):
                print("Invalid Option is selected")        
                c.close()
                break
        
        #Receiving Window size from client
        window_size = int(c.recv(100))
        print("Window Size : ",window_size)
        
        if(choice==1):
                message = raw_input("Enter a message : ")
                print("Go Back N Protocol is selected")
                sending_message(message)
                
                frame_drp_no = int(c.recv(5))
                print("NACK has been received for sequence no : ", frame_drp_no+1)
                split_message = [message[i:i+window_size] for i in range(frame_drp_no, len(message), window_size)]
                
                #Resends all frames from the point after which the frame for which NACK was received.
                print("Resending Message : ", split_message)
                #Resending the frames from the one after they are dropped
                for msg in split_message:
                        #Send message character by character
                        msg_split_frames = [msg[i:i+1] for i in range(0, len(msg), 1)]
                        for fm in msg_split_frames:
                                msg_frames.extend(fm)
                                c.send(fm)

        elif(choice==2):
                message = raw_input("Enter a message : ")
                print("Selective Repeat Protocol is selected")
                
                sending_message(message)
                
                frame_drp_no = int(c.recv(5))
                print("NACK has been received for sequence no : ", frame_drp_no+1)
                resend_frame = message[frame_drp_no]
                
                #Resends only that frame for which NACK was received
                print("Resending Message : ", resend_frame)
                c.send(resend_frame)

s.close()
