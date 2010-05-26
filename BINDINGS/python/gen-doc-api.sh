#!/bin/sh

mkdir -p doc/api/html

epydoc --html -o doc/api/html \
    -n "Python-EFL API" \
    evas.c_evas \
    evas.debug \
    evas.decorators \
    evas.utils \
    ecore.c_ecore \
    ecore.evas.c_ecore_evas \
    ecore.imf.c_ecore_imf \
    ecore.x.c_ecore_x \
    ecore.x.screensaver \
    ecore.evas.utils \
    edje.c_edje \
    edje.decorators \
    emotion.c_emotion \
    elementary.c_elementary \
    e_dbus

