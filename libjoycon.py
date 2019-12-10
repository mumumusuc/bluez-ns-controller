#!/usr/bin/python3

from ctypes import *

_libjc = CDLL('libcontroller.so')


class Device(Structure):
    _fields_ = [
        ('role', c_int),
        ('name', c_char_p),
        ('mac_address', c_char_p),
        ('serial_number', c_char_p),
    ]

    def __init__(self, device, *args, **kw):
        super().__init__(*args, **kw)
        self._device = device
        self._output_buffer = create_string_buffer(64)
        self._input_buffer = create_string_buffer(64)

        @CFUNCTYPE(c_int, POINTER(c_char), c_size_t)
        def _session_send(buffer, length):
            memmove(self._output_buffer, buffer, c_size_t(length))
            try:
                length = self._device.write(self._output_buffer.raw)
                #print('Send ->[{}] {}'.format(length, _output_buffer.raw))
                return length
            except IOError as e:
                print(e)
            except Exception as e:
                print(e)
            return -1

        @CFUNCTYPE(c_int, POINTER(c_char), c_size_t)
        def _session_recv(buffer, length):
            try:
                d = self._device.read(length)
                length, data = len(d), bytes(d)
                #print('Recv ->[{}] {}'.format(l, data))
                memmove(buffer, c_char_p(data), c_size_t(length))
                return length
            except IOError as e:
                print(e)
            except Exception as e:
                print(e)
            return -1

        self._session_send = _session_send
        self._session_recv = _session_recv
        create = _libjc.Session_create
        create.restype = c_void_p
        self._session = create(
            byref(self),
            self._session_recv,
            self._session_send,
            None, None
        )

    def __del__(self):
        _libjc.Session_release(self._session)


class Console(Device):
    def __init__(self, device):
        super().__init__(device, **{
            'role': 0,
            'name': 'Nintendo Switch'.encode('utf-8'),
            'mac_address': 'DC:68:EB:15:9A:62'.encode('utf-8'),
            'serial_number': 'XAJ70017696541'.encode('utf-8'),
        })

    def setHomeLight(self, pattern=None):
        _libjc.Console_setHomeLight(self._session)

    def setPlayerLight(self, player: int, flash: int = 0):
        _libjc.Console_setPlayerLight(
            self._session,
            c_int(player),
            c_int(flash)
        )


class Controller(Device):
    def __init__(self, device, *args, **kw):
        super().__init__(device, *args, **kw)


class JoyCon_L(Controller):
    def __init__(self, device, *args, **kw):
        super().__init__(device, *args, **kw)


class JoyCon_R(Controller):
    def __init__(self, device, *args, **kw):
        super().__init__(device, *args, **kw)


class ProController(Controller):
    def __init__(self, device, *args, **kw):
        super().__init__(device, *args, **kw)


if __name__ == '__main__':
    # do some test
    device = None
    console = Console(device=device)
    # console.test()
    console.setHomeLight()
