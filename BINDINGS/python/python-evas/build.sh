#!/bin/sh

PREFIX="$HOME/usr"
PYX_FILES="evas/evas.c_evas.pyx"

for arg in $@; do
    case $arg in
        --force)
            touch $PYX_FILES
            ;;
        *)
            PREFIX="$arg"
            ;;
    esac
done

python setup.py develop --prefix="$PREFIX" install_headers
