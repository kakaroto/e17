#!/bin/sh
sudo ifconfig usb0 192.168.0.200 netmask 255.255.255.0
sudo route add 192.168.0.202 dev usb0
sudo ./scripts/enable-ip-forward.sh
sudo iptables -t nat -A POSTROUTING -o eth0 -j MASQUERADE
sudo iptables -L
ssh root@192.168.0.202
