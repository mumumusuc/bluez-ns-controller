#!/usr/bin/python3

import os
import hid
import libjoycon
from time import sleep
from ctypes import *

JoyCon_R_ID = (1406, 8199)
libc = CDLL('libc.so.6')

_buffer = create_string_buffer(65)


@CFUNCTYPE(c_int, POINTER(c_char), c_size_t)
def Send(buffer, length):
    libc.memmove(_buffer, buffer, length)
    print('Send ->[{}] {}'.format(length, _buffer.raw))
    device.write(_buffer.raw)
    return length


@CFUNCTYPE(c_int, POINTER(c_char), c_size_t)
def Recv(buffer, length):
    d = device.read(length)
    l = len(d)
    data = bytes(d)
    print('Recv ->[{}] {}'.format(l, data))
    libc.memmove(buffer, c_char_p(data), l)
    return l
    #echo = 'Python!'.encode('utf-8')
    #libc.memmove(buffer, echo, len(echo))
    # return len(echo)


class Device(Structure):
    _fields_ = [
        ('role', c_int),
        ('name', c_char_p),
        ('mac_address', c_char_p),
        ('serial_number', c_char_p),
    ]


'''
static Device_t SwitchConsole = {
    .role = CONSOLE,
    .name = "Nintendo Switch",
    .mac_address = "DC:68:EB:15:9A:62",
    .serial_number = "XAJ70017696541",
};
'''

if __name__ == '__main__':
    '''
    libjc = cdll.LoadLibrary('libcontroller.so')
    libjc.createSession2.restype = c_void_p
    d = Device(**{
        'role': 0,
        'name': 'Nintendo Switch'.encode('utf-8'),
        'mac_address': 'DC:68:EB:15:9A:62'.encode('utf-8'),
        'serial_number': 'XAJ70017696541'.encode('utf-8'),
    })
    ref_d = byref(d)
    session = libjc.createSession2(ref_d, Recv, Send, None, None)
    # libjc.testSession2(session)

    for d in hid.enumerate(JoyCon_R_ID[0], JoyCon_R_ID[1]):
        keys = list(d.keys())
        keys.sort()
        for key in keys:
            print("%s : %s" % (key, d[key]))
        print()
    device = hid.device()
    device.open(JoyCon_R_ID[0], JoyCon_R_ID[1])
    # libjc._connect(session)
    libjc._setHomeLight(session)
    for i in range(4):
        libjc._setPlayer(session, 1 << i, 0)
        sleep(0.5)
    libjc.destroySession2(session)

    device.close()
    '''
    device = hid.device()
    device.open(JoyCon_R_ID[0], JoyCon_R_ID[1])
    # device.set_nonblocking(True)

    console = libjoycon.Console(device=device)
    info = console.getControllerInfo()
    print(info.firmware, info.category, info.mac_address)
    voltage = console.getControllerVoltage()
    print('voltage : ', hex(voltage))
    patterns = [
        libjoycon.HomeLightPattern(0xF, 0, 0xF),
        libjoycon.HomeLightPattern(0, 0, 0xF),
    ]
    console.setHomeLight(0x0, 0x3, 0x5, patterns)
    console.testPoll()
    sleep(5)
    #console.stopPoll()
    #sleep(5)
    device.close()
    print('done')
