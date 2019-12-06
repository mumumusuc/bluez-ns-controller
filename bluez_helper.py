#!/usr/bin/python3

import dbus

_SERVICE = 'org.bluez'
_MANAGERS = '/org/bluez'
_I_OBJ_MANAGER = 'org.freedesktop.DBus.ObjectManager'
_I_PRF_MANAGER = '{}.ProfileManager1'.format(_SERVICE)
_I_AGT_MANAGER = '{}.AgentManager1'.format(_SERVICE)
_I_PROPERTIES = 'org.freedesktop.DBus.Properties'
_I_ADAPTER = '{}.Adapter1'.format(_SERVICE)
_I_DEVICE = '{}.Device1'.format(_SERVICE)


class _Prop:
    def __init__(self, iface):
        self.__dict__['_interface'] = iface.dbus_interface
        self.__dict__['_prop'] = dbus.Interface(iface, _I_PROPERTIES)

    def __getattr__(self, item):
        return self.__dict__['_prop'].Get(self._interface, item)

    def __setattr__(self, key, value):
        self.__dict__['_prop'].Set(self._interface, key, value)


class BluezHelper:
    def __init__(self):
        self._bus = dbus.SystemBus()
        self._root = self._bus.get_object(_SERVICE, '/')
        self._managers = self._bus.get_object(_SERVICE, _MANAGERS)
        obj_manager = dbus.Interface(self._root, _I_OBJ_MANAGER)
        self._adapters = []
        self._devices = []
        for p, i in obj_manager.GetManagedObjects().items():
            if _I_ADAPTER in i.keys():
                adapter = i.get(_I_ADAPTER)
                self._adapters.append({p: adapter})
            elif _I_DEVICE in i.keys():
                device = i.get(_I_DEVICE)
                self._devices.append({p: device})
        self.adapter_count = len(self._adapters)
        self.device_count = len(self._devices)

    def list_adapters(self):
        for i, a in enumerate(self._adapters):
            for k, v in a.items():
                print('Adapter_{} [{}]:\n'.format(i, k), BluezHelper._fmt_interface(v))

    def list_devices(self):
        for i, a in enumerate(self._devices):
            for k, v in a.items():
                print('Device_{} [{}]:\n'.format(i, k), BluezHelper._fmt_interface(v))

    def get_adapter(self, index: int = 0):
        if index < 0 or index >= self.adapter_count:
            return None
        for path in self._adapters[index].keys():
            obj = self._bus.get_object(_SERVICE, path)
            adapter = dbus.Interface(obj, _I_ADAPTER)
            adapter.prop = _Prop(adapter)
            return adapter

    def get_device(self, index: int = 0):
        if index < 0 or index >= self.device_count:
            return None
        for path in self._devices[index].keys():
            obj = self._bus.get_object(_SERVICE, path)
            device = dbus.Interface(obj, _I_DEVICE)
            device.prop = _Prop(device)
            return device

    def get_agent_manager(self):
        return dbus.Interface(self._managers, _I_AGT_MANAGER)

    def get_profile_manager(self):
        return dbus.Interface(self._managers, _I_PRF_MANAGER)

    @classmethod
    def _fmt_interface(cls, iface):
        s = ''
        for k, v in iface.items():
            if k == 'UUIDs':
                s += '\tUUIDs:\n'
                for i, u in enumerate(v):
                    s += '\t\t{} : {}\n'.format(i, u)
            elif k == 'Class':
                s += '\t%s : 0x%06x\n' % (k, v)
            elif k in ('Vendor', 'Product', 'Version'):
                s += '\t%s : 0x%04x\n' % (k, v)
            else:
                s += '\t{} : {}\n'.format(k, v)
        return s


if __name__ == '__main__':
    # do some test
    pass
