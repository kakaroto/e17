#!/bin/sh
PREFIX="$HOME/usr"

for arg in $@; do
    case $arg in
        *)
            PREFIX="$arg"
            ;;
    esac
done

python setup.py install --prefix="$PREFIX"
