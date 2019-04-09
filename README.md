# BLUETERM5 - M5Stack Core Bluetooth to TFT terminal.
This sketch makes your M5 Core into a remote bluetooth terminal.

Credit goes to Bodmer for the example code this is based on: https://github.com/Bodmer/TFT_eSPI/tree/master/examples/320%20x%20240/TFT_Terminal. 

This MAIN m5stack branch is for work on integrating into M5Stack.

* Push Button A for no display auto-sleep (i.e. NoZ, default). Long-Push Button A to turn on display auto-sleep (ZZZ).
* Push Button C to force wake display. Long-Push Button C to force sleep display.
* NOTE: *DISPLAY WILL AUTO-WAKE ON ANY INPUT.*

* Battery status upper right
* Auto-Sleep status in upper left.

In Arduino, set Tool->Partition Scheme-> No OTA (Large APP)

Linux Example:
```
$ sudo bluetoothctl
[bluetooth]# agent on
[bluetooth]# scan on
  [NEW] Device 08:3E:8E:E6:79:47 annapurna
  [NEW] Device 00:25:56:D1:36:6B ubuntu-0
  ....
  [NEW] Device <bluetooth address> BLUETERM5
[bluetooth]# pair <bluetooth address>
[bluetooth]# trust <bluetooth address>
CTRL-D
$ sudo rfcomm connect /dev/rfcomm0 <bluetooth address> 1
$ tail -f /var/log/syslog > /dev/rfcomm0
```

![Example1](images/IMG_20190406_143541.jpg)
![Example2](images/IMG_20190406_143720.jpg)
