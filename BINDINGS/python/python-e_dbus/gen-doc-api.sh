#!/bin/sh

mkdir -p doc/api/html

epydoc --html -o doc/api/html \
    -n "Python-E_Dbus API" \
    e_dbus
