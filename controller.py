#!/usr/bin/python3

import os
import sys
import uuid
import dbus.service
from time import sleep
from bluez_helper import BluezHelper
from socket import SOCK_SEQPACKET, AF_BLUETOOTH, BTPROTO_L2CAP, BDADDR_ANY, socket
from dbus.mainloop.glib import DBusGMainLoop
from gi.repository import GLib

_PROFILE = 'org.bluez.Profile1'


class HidService:
    _HID_UUID = '00001124-0000-1000-8000-00805f9b34fb'
    _HID_PATH = '/mumumusuc/hid/pro'
    _PSM_CRTL = 17
    _PSM_INTR = 19

    def __init__(self, profile_path):
        self._path = profile_path
        self._hci_name = 'Pro Controller'
        self._hci_class = '0x002508'
        self._uuid = str(uuid.uuid4())
        self._intr = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP)
        self._ctrl = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP)

    def _on_intr_socket(self, sock, cond):
        print('on_intr_socket')
        sco, peer = sock.accept()
        print('new connection from {}'.format(peer))
        sco.settimeout(2)
        data = [0 for _ in range(65)]
        data[0] = 0xA1
        for _ in range(10):
            sco.send(bytes(data))
            try:
                print(sco.recvfrom(65))
            except Exception as e:
                print(e)
            sleep(0.5)

    def _on_ctrl_socket(self, sock, cond):
        print('on_ctrl_socket')
        sco, peer = sock.accept()
        print('new connection from {}'.format(peer))
        # we do not need this socket
        pass

    def listen(self):
        self._intr.bind((BDADDR_ANY, self._PSM_INTR))
        self._ctrl.bind((BDADDR_ANY, self._PSM_CRTL))
        self._intr.listen(1)
        self._ctrl.listen(1)
        GLib.io_add_watch(self._intr, GLib.IO_IN, self._on_intr_socket)
        #GLib.io_add_watch(self.intr, GLib.IO_OUT, self.on_intr_socket)
        GLib.io_add_watch(self._ctrl, GLib.IO_IN, self._on_ctrl_socket)
        #GLib.io_add_watch(self.ctrl, GLib.IO_OUT, self.on_ctrl_socket)

    def register(self):
        with open(self._path, 'r') as p:
            opts = {
                # 'Name':'Pro Controller',
                'ServiceRecord': p.read(),
                'Role': 'server',
                'Service': self._HID_UUID,
                'RequireAuthentication': False,
                'RequireAuthorization': False
            }
            h = BluezHelper()
            pm = h.get_profile_manager()
            pm.RegisterProfile(self._HID_PATH, self._uuid, opts)


if __name__ == '__main__':
    DBusGMainLoop(set_as_default=True)
    loop = GLib.MainLoop()
    hid = HidService(profile_path='profile/sdp_record_hid_pro.xml')
    hid.listen()
    print('restart bluetooth ...')
    os.system('systemctl restart bluetooth.service')
    sleep(1)
    print('set class to 0x002508 ...')
    os.system('hciconfig hci0 class 0x002508')
    print('set name to "Pro Controller" ...')
    os.system('hciconfig hci0 name "Pro Controller"')
    helper = BluezHelper()
    adapter = helper.get_adapter()
    adapter.prop.Powered = True
    adapter.prop.Discoverable = True
    print('register profile ...')
    hid.register()
    print('waiting for connection ...')
    loop.run()
