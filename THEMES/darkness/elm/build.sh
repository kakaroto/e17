#!/bin/bash

edje_cc -id . -id ../elm_default/themes -id ../etk -fd ../fonts  darkness-desktop.edc darkness-elm-desktop.edj
edje_cc -id . -id ../elm_default/themes -id ../etk -fd ../fonts  darkness.edc darkness-elm.edj
case "$1" in
    '-p')
    edje_viewer darkness-elm.edj
    ;;
    '-i')
    cp darkness-elm{,-desktop}.edj ~/.elementary/themes/
    ;;
esac
