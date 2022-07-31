import socket

def main():
    server_addr = ('localhost', 2300)
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    try:
        s.connect(server_addr)
        print("Connected to {:s}".format(repr(server_addr)))
        
        s.send(b'Hello World')
            
    except AttributeError as ae:
        print("Error creating the socket: {}".format(ae))
    except socket.error as se:
        print("Exception on socket: {}".format(se))
    finally:
        print("Closing socket")
        s.close()


if __name__ == "__main__":
    main()