#!/usr/bin/python3

import os
from ctypes import *


def Print(str):
    print(str)
    return 0


if __name__ == '__main__':
    libc = cdll.LoadLibrary('libc.so.6')  # Load standard C library on Linux
    print(libc.time(None))

    libjc = cdll.LoadLibrary('libcontroller.so')
    # libjc.main()
    C_CALLBACK_FUNC = CFUNCTYPE(c_int, c_char_p)
    callback = C_CALLBACK_FUNC(Print)
    #libjc.python_call.restype = c_voidp
    libjc.python_call(callback)
