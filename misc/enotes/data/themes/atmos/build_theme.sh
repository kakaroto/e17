#!/bin/sh
THEME="atmos"
if [ -f $THEME.eet ]; then
    rm -f $THEME.eet
fi
edje_cc -v -id ./images $THEME.edc $THEME.eet
edje $THEME.eet
