#!/bin/sh

epydoc --html -o doc/api/html \
    -n "Python-Epsilon API" \
    epsilon.c_epsilon \
    epsilon.request
