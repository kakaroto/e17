#!/bin/sh -e

if [ "x$1" != "x" ]; then
    SRCDIR=$1
else
    SRCDIR=.
fi
edje_cc -v -id $SRCDIR/img $SRCDIR/default.edc $SRCDIR/default.eet
