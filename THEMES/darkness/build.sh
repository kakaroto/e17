#!/bin/bash
edje_cc -id images/ -fd . darkness.edc -o darkness.edj
[[ $? != 0 ]] && exit 1
case "$1" in
    '-p')
    edje_viewer darkness.edj
    ;;
    '-i')
    cp darkness.edj ~/.e/e/themes/
    ;;
    '-r')
    cp darkness.edj ~/.e/e/themes/
    enlightenment_remote -restart
    ;;
esac
