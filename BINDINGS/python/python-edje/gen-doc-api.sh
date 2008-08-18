#!/bin/sh

epydoc --html -o doc/api/html \
    -n "Python-Edje API" \
    edje.c_edje \
    edje.decorators
