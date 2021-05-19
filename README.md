# surveillant
## Cluster Stat Management Project

![surveillant](lv_resources/surveillant.png "surveillant")
surveillant ~(french): supervisor

made with LVGL configured to work with /dev/fbX on Linux.

> currently uses LVGLv7

aims to monitor and observe the statistics of a linux based cluster by showing the stats on a screen attached to linux framebuffer `/dev/fbX`
___

basics: 
* this project is configured to use `/dev/fb0` 1080x1920 screen and `/dev/input/event3`, more info on how to configure is given below
* the screen UI is made for portrait framebuffer, 

future checkpoints:
- [] optimise the configuration file 
- [] adapt to all screen sizes, use fractional scaling

___

### configuring libraries:
```bash
mkdir lv_drivers
mkdir lv_examples
cd lv_drivers
git pull && git checkout release/v7
cd .. 
cd lvgl
git pull && git checkout release/v7
cd ..
```

### building the code:
```bash
make -F Makefile
```

### running it:
```bash
./main_app
```

### cleaning up:
```bash
make clean
```
___ 
## Configuring surveillant to your system:

### screen configuration:

to configure surveillant to your screen you must configure it to the specific framebuffer screen and the touch input on your system

check `lv_drv_conf.h` for something like:

`#  define FBDEV_PATH          "/dev/fbX"` for the framebuffer 

`#  define EVDEV_NAME   "/dev/input/eventX"` for the touch input

here 'X' can vary according to your system, framebuffer `/dev/fb0` is most commonly used, so it is the touch input that needs configuring, use the tool "evtest" to find the appropriate input channel, eg. `/dev/input/event3`

### cluster node configuration:
[tbd]