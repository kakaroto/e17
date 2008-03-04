#!/bin/sh

PREFIX="$HOME/usr"
PYX_FILES="ecore/ecore.c_ecore.pyx ecore/evas/ecore.evas.c_ecore_evas.pyx ecore/x/ecore.x.c_ecore_x.pyx ecore/x/ecore.x.screensaver.pyx ecore/imf/ecore.imf.c_ecore_imf.pyx"

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

PYINCDIR=`python -c "import distutils.sysconfig; print distutils.sysconfig.get_python_inc()"`
INCDIR="$(dirname $PREFIX)/$PYINCDIR"

python setup.py build_ext --include-dirs="$INCDIR" develop --prefix="$PREFIX" install_headers --install-dir="$INCDIR"
