#!/bin/sh
THEME="taillights"
if [ -f $THEME.eet ]; then
    rm -f $THEME.eet
fi
edje_cc -v -id ./images $THEME.edc $THEME.eet
#edje $THEME.eet
#sudo cp -f $THEME.eet /usr/share/entrance/themes/
