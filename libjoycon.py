#!/usr/bin/python3

from ctypes import *

_libjc = CDLL('libcontroller.so')


def hexs(data: bytes):
    return ' '.join(['%02x' % i for i in data])


class ControllerInfo(Structure):
    _fields_ = [
        ('firmware', c_uint8*2),
        ('category', c_uint8),
        ('_', c_uint8),
        ('mac_address', c_uint8*6),
    ]

    def __getattribute__(self, name):
        field = super().__getattribute__(name)
        if name == 'firmware':
            field = ((field[0] << 8) & 0xFF00) | (field[1] & 0x00FF)
        elif name == 'category':
            pass
        elif name == 'mac_address':
            field = ':'.join(['%02x' % b for b in field])
        return field


class HomeLightPattern(Structure):
    _fields_ = [
        ('intensity', c_uint8),
        ('duration', c_uint8),
        ('transition', c_uint8),
    ]

    def __init__(self, intensity: int, duration: int, transition: int):
        super().__init__(**{
            'intensity': c_uint8(intensity),
            'duration': c_uint8(duration),
            'transition': c_uint8(transition),
        })


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
        self._buffer = create_string_buffer(64)

        @CFUNCTYPE(c_int, POINTER(c_char), c_size_t)
        def _session_send(buffer, length):
            memmove(self._buffer, buffer, c_size_t(length))
            try:
                data = self._buffer.raw
                length = self._device.write(data)
                print('Send[{}]{}'.format(length, hexs(data)))
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
                print('Recv[{}] {}'.format(length, hexs(data)))
                memmove(buffer, c_char_p(data), c_size_t(length))
                return length
            except IOError as e:
                print('_session_recv : ', e)
            except Exception as e:
                print('_session_recv : ', e)
            return -1

        self.session_send = _session_send
        self.session_recv = _session_recv
        create = _libjc.Session_create
        create.restype = c_void_p
        self._session = create(
            byref(self),
            _session_recv,
            _session_send,
            None, None
        )

    def _session_send(self):
        self.session_send

    def _session_recv(self):
        self.session_recv

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

    def getControllerInfo(self):
        info = ControllerInfo()
        _libjc.Console_getControllerInfo(self._session, byref(info))
        return info

    def getControllerVoltage(self):
        voltage = c_uint16()
        _libjc.Console_getControllerVoltage(self._session, byref(voltage))
        return voltage.value

    def setHomeLight(self, intensity=0, duration=0,  repeat=0, patterns=None):
        #_libjc.get_double_blink_pattern.restype = POINTER(HomeLightPattern)
        #pattern = _libjc.get_double_blink_pattern()
        Console_setHomeLight = _libjc.Console_setHomeLight
        size = len(patterns)
        PatternArray = HomeLightPattern*size
        Console_setHomeLight.argtype = [
            c_uint8, c_uint8, c_uint8, c_size_t, POINTER(HomeLightPattern)
        ]
        patternArray = PatternArray()
        for i, p in enumerate(patterns):
            patternArray[i] = patterns[i]
        Console_setHomeLight(
            self._session,
            c_uint8(intensity),
            c_uint8(duration),
            c_uint8(repeat),
            c_size_t(size),
            byref(patternArray),
        )

    def setPlayerLight(self, player: int, flash: int = 0):
        _libjc.Console_setPlayerLight(
            self._session,
            c_int(player),
            c_int(flash)
        )

    def test(self):
        print(super._session_recv())


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
    console.test()
    # console.setHomeLight()
