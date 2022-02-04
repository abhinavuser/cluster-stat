#!/usr/bin/bash
cd $HOME
sleep 10
while true;do echo 0 > /sys/class/graphics/fbcon/cursor_blink;./surveillant;sleep 1;done
