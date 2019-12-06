# bluez-ns-controller
使用bluez模拟Nintendo Switch的Pro手柄(Use bluez to emulate a Nintendo Switch Pro-Controller)
分离自Android端的模拟手柄项目[Bluetooth-Joycon](https://github.com/mumumusuc/Bluetooth-Joycon)，专注于在linux或其他支持bluez5的平台。

**在我的rpi-3b(RaspbianLite, bluez5.43)，rpi-0w(RaspbianLite, bluez5.43)，PC(Pop!_OS 19.10, bluez5.50)成功测试**

## Config

1. set bluetooth PID&VID
  ```
  @ /etc/bluetooth/main.conf
  
  + DeviceID=bluetooth:057e:2009:200
  ```

2. set bluetoothd
  ```
  @ /etc/systemd/system/dbus-org.bluez.service
  
  - ExecStart=/usr/lib/bluetooth/bluetoothd
  + ExecStart=/usr/lib/bluetooth/bluetoothd -C
  ```

## Dependencies

> TBD

## Run

```
$ sudo python3 controller.py

> ...
```

## Referrence
在Android平台上模拟Pro手柄            :[Bluetooth-Joycon](https://github.com/mumumusuc/Bluetooth-Joycon)

依据逆向工程制作的Joycon库             :[LibJoycon](https://github.com/mumumusuc/LibJoycon)

linux平台上模拟Pro和Hori-S有线手柄和声卡驱动  :[pi-joystick](https://github.com/mumumusuc/pi-joystick)

Joycon逆向工程                       :[Nintendo_Switch_Reverse_Engineering](https://github.com/dekuNukem/Nintendo_Switch_Reverse_Engineering)

window平台的jc_toolkit               :[jc_toolkit](https://github.com/CTCaer/jc_toolkit)

linux版的jc_toolkit                  :[jc-toolkit-linux](https://github.com/mumumusuc/jc-toolkit-linux)
