#!/bin/bash
v=$(svnversion -n .|grep -Eo "^[0-9]+")
[[ -z $v ]] && v=$(git svn log --oneline --limit=1 .|cut -d' ' -f1|sed 's/r//' | tr -d '\n')
edje_cc -DVERSION=\"$v\" -id images/ -fd . darkness.edc -o darkness.edj
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
