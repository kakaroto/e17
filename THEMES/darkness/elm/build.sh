#!/bin/bash

edje_cc -id . -fd ../fonts  darkness-desktop.edc darkness-elm-desktop.edj
[[ $? != 0 ]] && exit 1
edje_cc -id . -fd ../fonts  darkness.edc darkness-elm.edj
[[ $? != 0 ]] && exit 1
case "$1" in
    '-p')
    edje_viewer darkness-elm.edj
    ;;
    '-i')
    cp darkness-elm{,-desktop}.edj ~/.elementary/themes/
    ;;
esac
