#!/bin/sh

epydoc --html -o doc/api/html -n "Python-Ecore API" ecore.c_ecore ecore.evas.c_ecore_evas ecore.evas.utils
