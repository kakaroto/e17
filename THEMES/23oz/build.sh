#!/bin/bash
edje_cc -id images -fd . default.edc -o 23oz.edj
[[ $? != 0 ]] && exit 1
case "$1" in
    '-p')
    edje_viewer 23oz.edj
    ;;
    '-i')
    cp 23oz.edj ~/.e/e/themes/
    ;;
    '-r')
    cp 23oz.edj ~/.e/e/themes/
    enlightenment_remote -restart
    ;;
esac
