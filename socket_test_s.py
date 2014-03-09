#!/usr/bin/env python

import socket
import sockethandler
import serialization
from time import sleep

def callback(uid, object):
    if object:
        print("Callback OK.")
        print(object.kp)
    else:
        print("No object :(")

handler = sockethandler.SocketHandlerSender()

serv_sock = socket.socket()
serv_sock.bind(('127.0.0.1', 5000))
serv_sock2 = socket.socket()
serv_sock2.bind(('127.0.0.1', 6000))

serv_sock.listen(1)
serv_sock2.listen(1)
s1, addr1 = serv_sock.accept()
print(addr1)
s1.setblocking(0)
s2, addr2 = serv_sock2.accept()
print(addr2)
s2.setblocking(0)

m1 = serialization.PIDConfig()
m2 = serialization.PIDConfig()
m1.kp = 41
m2.kp = 42
print(m1.kp)
print(m2.kp)



try:
    while True:
        print("Sending.")
        handler.send_package(s1, m1)
        handler.send_package(s1, m2)
        handler.send_package(s2, m1)
        handler.send_package(s2, m2)
        print("Packages in buffer: " + str(handler.send()))
        sleep(1)
except:
    print("Interrupted.")
    s1.close()
    s2.close()
    serv_sock.close()
    serv_sock2.close()
