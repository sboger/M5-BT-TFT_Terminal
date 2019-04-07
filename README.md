# BLUETERM5 - M5Stack Core Bluetooth to TFT terminal.
This sketch makes your M5 Core into a remote bluetooth terminal.

Starting Code framework nearly entirely from https://github.com/Bodmer/TFT_eSPI/tree/master/examples/320%20x%20240/TFT_Terminal. 

This MAIN m5stack branch is for work on integrating into M5Stack.

* LCD_BRIGHTNESS setting in sketch 
* Battery status upper right
* Display will autosleep after SECS_TO_SLEEP setting in sketch.
* Push Button C to wake display.
* Long-Push Button C to sleep display.
* *DISPLAY WILL AUTO-WAKE ON ANY INPUT.*

In Arduino, set Tool->Partition Scheme-> No OTA (Large APP)

Linux Example:
```
$ sudo bluetoothctl
[bluetooth]# agent on
[bluetooth]# scan on
  [NEW] Device 08:3E:8E:E6:79:47 annapurna
  [NEW] Device 00:25:56:D1:36:6B ubuntu-0
  ....
  [NEW] Device <bluetooth address> BLUETERM
[bluetooth]# pair <bluetooth address>
[bluetooth]# trust <bluetooth address>
CTRL-D
$ sudo rfcomm connect /dev/rfcomm0 <bluetooth address> 1
$ tail -f /var/log/syslog > /dev/rfcomm0
```

![Example1](images/IMG_20190406_143541.jpg)
![Example2](images/IMG_20190406_143720.jpg)
