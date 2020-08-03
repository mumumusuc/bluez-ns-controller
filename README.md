# bluez-ns-controller
使用bluez模拟Nintendo Switch的Pro手柄(Use bluez to emulate a Nintendo Switch Pro-Controller)
分离自Android端的模拟手柄项目[Bluetooth-Joycon](https://github.com/mumumusuc/Bluetooth-Joycon)，专注于在linux或其他支持bluez5的平台。

**在我的rpi-3b(RaspbianLite, bluez5.43)，rpi-0w(RaspbianLite, bluez5.43)，PC(Pop!_OS 19.10, bluez5.50)成功测试**

## Note

1. 由于bluez5抛弃了很多旧版本的工具，所以编程方法应该变得更加“现代”，要遵循bluez doc的api进行编程。

2. 事实上在一些ArchLinux上是可以直接bind本机蓝牙地址的，不需要重启bluetoothd。

3. 对于以上问题，深究一下其实是bluez“贴心”的包揽了HID/HIDP的处理工作，而导致了用户的HID服务端无法运行。在bluez4在main.conf里有`DisablePlugins = input`的设置，而bluez5取消了这个config，使用`systemctl status bluetooth.service`查看蓝牙状态会发现bluez5不认这一设置。使用`bluetoothd -h`查看帮助项可知`-P, --noplugin=NAME,...     Specify plugins not to load`,所以在启动bluetoothd时指定`-P input`即可停止bluez5的HID服务让自己的程序正常运行。

参考资料：

  [mplementing a Bluetooth HID Profile on the Edison](https://community.intel.com/t5/Intel-Makers/Implementing-a-Bluetooth-HID-Profile-on-the-Edison/td-p/546841)

  [iikeyboard](https://github.com/ii/iikeyboard/wiki) 


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
  + ExecStart=/usr/lib/bluetooth/bluetoothd -P input
  ```

## Dependencies

python3-dbus python3-glib python3-evdev

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
