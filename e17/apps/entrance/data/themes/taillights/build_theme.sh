#!/bin/sh
THEME="taillights"
if [ ! -f $THEME.eet ]; then
	edje_cc -v -id ./images $THEME.edc $THEME.eet
fi
#edje $THEME.eet
#sudo cp -f $THEME.eet /usr/share/entrance/themes/
