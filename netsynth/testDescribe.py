#!/usr/bin/env python

import socket
import struct
import connector_pb2

def sendChunk(s, message):
    messageLen = len(message)
    print("messageLen =", messageLen)
    data = struct.pack('i', socket.htonl(messageLen))
    s.send(data)
    s.send(message)

def receiveChunk(s):
    data = s.recv(4)
    if not data:
        return
    unpacked = struct.unpack('i', data)
    length = socket.ntohl(unpacked[0])
    print(length)
    message = s.recv(length)
    return message

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(('localhost', 12345))

request = connector_pb2.Request()
request.command = connector_pb2.Request.DESCRIBE
data = request.SerializeToString()
sendChunk(s, data)

replyData = receiveChunk(s)

reply = connector_pb2.Reply()
reply.ParseFromString(replyData)

if reply.status == connector_pb2.Reply.SUCCESS:
    print(reply)
    # for attr in reply.component.attribute:
    #    print("  ", attr.name, "=", attr.value)
    print("OK success")
else:
    print("NO fail")
