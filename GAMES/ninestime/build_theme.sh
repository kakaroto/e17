#!/bin/bash

DATADIR="`dirname $0`/data/theme"

EDJE_CC=edje_cc
EDJE_FLAGS="-v -id $DATADIR/images -fd $DATADIR/fonts"

$EDJE_CC $EDJE_FLAGS $DATADIR/default.edc $DATADIR/default.edj
