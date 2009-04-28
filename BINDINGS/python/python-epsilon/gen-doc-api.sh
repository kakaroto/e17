#!/bin/bash

pyver=`python -c 'import sys; print "%d.%d" % sys.version_info[0:2]'`
regex=lib\.[a-z0-9]*-[a-z0-9]*-$pyver

for d in `ls build`; do
    if [[ $d == $regex ]]; then
        break
    fi
done

export PYTHONPATH="`pwd`/build/$d"

epydoc --html -o doc/api/html \
    -n "Python-Epsilon API" \
    epsilon.c_epsilon \
    epsilon.request
