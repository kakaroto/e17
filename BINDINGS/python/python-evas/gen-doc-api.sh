#!/bin/sh

epydoc --html -o doc/api/html \
    -n "Python-Evas API" \
    evas.c_evas \
    evas.debug \
    evas.decorators \
    evas.utils
