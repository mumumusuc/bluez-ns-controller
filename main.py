#!/usr/bin/python3

import os
import hid
import libjoycon
from time import sleep
from ctypes import *

JoyCon_R_ID = (1406, 8199)

if __name__ == '__main__':
    device = hid.device()
    device.open(JoyCon_R_ID[0], JoyCon_R_ID[1])
    # device.set_nonblocking(True)

    console = libjoycon.Console(device=device)
    #info = console.getControllerInfo()
    #print(info.firmware, info.category, info.mac_address)
    #voltage = console.getControllerVoltage()
    #print('voltage : ', hex(voltage))
    patterns = [
        libjoycon.HomeLightPattern(0xF, 0, 0xF),
        libjoycon.HomeLightPattern(0, 0, 0xF),
    ]
    console.testPoll()
    sleep(1)
    console.setPlayerLight(1)
    console.setHomeLight(0x0, 0x3, 0x5, patterns)
    sleep(1)
    console.stopPoll()
    sleep(3)
    device.close()
    print('done')
