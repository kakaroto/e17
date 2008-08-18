#!/bin/sh

epydoc --html -o doc/api/html \
    -n "Python-Ecore API" \
    ecore.c_ecore \
    ecore.evas.c_ecore_evas \
    ecore.imf.c_ecore_imf \
    ecore.x.c_ecore_x \
    ecore.x.screensaver \
    ecore.evas.utils
