import socket
import threading

HOST = socket.gethostbyname('localhost')
PORT = 9091

clients = []


class WorkerThread(threading.Thread):
    """ A class to handle simultaneous multiuser connections """

    def __init__(self, client_sock):
        super().__init__()
        self.s = client_sock
        self.username = ''

    def run(self):
        self.username = bytes.decode(self.s.recv(100))
        print('Username: {}'.format(self.username))
        self.s.send(str.encode('Welcome to char server! Enter !exit to close chat.'))
        while True:
            try:
                data = bytes.decode(self.s.recv(100, socket.MSG_DONTWAIT))
                if data:
                    if '!exit' in data:
                        clients.remove(self.s)
                        break
                    data = self.username + '> ' + data
                    for c in clients:
                        if c != self.s:
                            try:
                                c.send(str.encode(data))
                            except (BrokenPipeError, ConnectionResetError) as e:
                                pass
            except (BlockingIOError, ConnectionResetError, BrokenPipeError) as e:
                if isinstance(e, ConnectionResetError) or isinstance(e, BrokenPipeError):
                    clients.remove(self.s)
                    break


if __name__ == '__main__':

    s = socket.socket(family=socket.AF_INET)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind((HOST, PORT))

    s.listen(5)

    while True:
        client, addr = s.accept()
        print('Client connected: {} '.format(addr))
        clients.append(client)
        WorkerThread(client).start()
