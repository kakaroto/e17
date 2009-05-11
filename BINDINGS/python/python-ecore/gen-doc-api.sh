#!/bin/bash

pyver=`python -c 'import sys; print "%d.%d" % sys.version_info[0:2]'`
regex=lib\.[a-z0-9]*-[a-z0-9]*-$pyver
destdir="doc/api/html"

for d in `ls build`; do
    if [[ $d == $regex ]]; then
        break
    fi
done

export PYTHONPATH="`pwd`/build/$d"

mkdir -p $destdir

epydoc --html -o $destdir \
    -n "Python-Ecore API" \
    ecore.c_ecore \
    ecore.evas.c_ecore_evas \
    ecore.imf.c_ecore_imf \
    ecore.x.c_ecore_x \
    ecore.x.screensaver \
    ecore.evas.utils
