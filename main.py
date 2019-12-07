#!/usr/bin/python3

import os
from ctypes import cdll

if __name__ == '__main__':
    libc = cdll.LoadLibrary('libc.so.6')  # Load standard C library on Linux
    print(libc.time(None))

    libjc = cdll.LoadLibrary('libcontroller.so')
    libjc.main()
