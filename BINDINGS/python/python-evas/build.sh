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

PYINCDIR=`python -c "import distutils.sysconfig; print distutils.sysconfig.get_python_inc()" | cut -d/ -f3-`
INCDIR="$PREFIX/$PYINCDIR"

python setup.py develop --prefix="$PREFIX" install_headers --install-dir="$INCDIR"
