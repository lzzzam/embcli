#!/usr/bin/env python3

""" client.py - Echo client for sending/receiving C-like structs via socket
References:
- Ctypes: https://docs.python.org/3/library/ctypes.html
- Sockets: https://docs.python.org/3/library/socket.html
"""

import socket
import sys
import random
from ctypes import *


""" This class defines a C-like struct """
class Payload(Structure):
    _fields_ = [("length", c_uint8),
                ("group", c_uint8),
                ("id", c_uint8),
                ("data", c_uint8)]


def main():
    server_addr = ('localhost', 2300)
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    try:
        s.connect(server_addr)
        print("Connected to {:s}".format(repr(server_addr)))

        for i in range(5):
            print("")
            payload_out = Payload(4, 0, 1, 0)
            print("Sending length={:d}, group={:d}, id={:d}, data={:d}".format(payload_out.length, payload_out.group, payload_out.id, payload_out.data))
            nsent = s.send(payload_out)
            # Alternative: s.sendall(...): coontinues to send data until either
            # all data has been sent or an error occurs. No return value.
            print("Sent {:d} bytes".format(nsent))

            # buff = s.recv(sizeof(Payload))
            # payload_in = Payload.from_buffer_copy(buff)
            # print("Received id={:d}, counter={:d}, temp={:f}".format(payload_in.id,
            #                                                    payload_in.counter,
            #                                                    payload_in.temp))
    except AttributeError as ae:
        print("Error creating the socket: {}".format(ae))
    except socket.error as se:
        print("Exception on socket: {}".format(se))
    finally:
        print("Closing socket")
        s.close()


if __name__ == "__main__":
    main()