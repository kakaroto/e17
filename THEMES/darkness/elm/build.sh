#!/bin/bash

edje_cc -id . -fd ../fonts  darkness-desktop.edc darkness-desktop.edj
[[ $? != 0 ]] && exit 1
edje_cc -id . -fd ../fonts  darkness.edc darkness.edj
[[ $? != 0 ]] && exit 1
cd config && ./build.sh $@
cd ..
case "$1" in
    '-p')
    edje_viewer darkness.edj
    ;;
    '-i')
    cp darkness{,-desktop}.edj ~/.elementary/themes/
    ;;
esac
