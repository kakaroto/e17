#!/bin/sh

# This is a quick and dirty script to 

Xephyr :1 -noreset -ac -br -dpi 283 -screen 480x640x16 &
#Xephyr :1 -noreset -ac -br -dpi 283 -screen 640x480x16 &
#Xephyr :1 -noreset -ac -br -dpi 283 -screen 480x640 &
#Xephyr :1 -noreset -ac -br -dpi 186 -screen 272x480 &
#Xephyr :1 -noreset -ac -br -dpi 141 -screen 240x320 &

sleep 1
export DISPLAY=:1
xmodmap -e "keycode 223 = 0x11000601"
xmodmap -e "keycode 224 = 0x11000602"
xmodmap -e "keycode 225 = 0x11000603"
xmodmap -e "keycode 226 = 0x11000604"
xmodmap -e "keycode 227 = 0x11000605"
xmodmap -e "keycode 228 = 0x11000606"
xmodmap -e "keycode 229 = 0x11000607"
xmodmap -e "keycode 230 = 0x11000608"
xmodmap -e "keycode 231 = 0x11000609"
xmodmap -e "keycode 232 = 0x1100060A"
xmodmap -e "keycode 232 = 0x1100060B"
xmodmap -e "keycode 233 = 0x1100060B"
xmodmap -e "keycode 232 = 0x1100060A"
xmodmap -e "keycode 234 = 0x1100060C"
xmodmap -e "keycode 235 = 0x1100060D"
xmodmap -e "keycode 236 = 0x1100060E"
xmodmap -e "keycode 237 = 0x1100060F"
xmodmap -e "keycode 238 = 0x11000610"
xmodmap -e "keycode 239 = 0x1008ff26"
#unset E_RESTART E_START E_IPC_SOCKET E_START_TIME
enlightenment_start -profile illume
