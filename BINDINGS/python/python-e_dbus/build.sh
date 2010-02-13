#!/bin/sh

C_FILES="module.c"

usage() {
    cat <<EOF
Usage:

    $0 <options>

where options:

    --force       force rebuild of whole tree, will not trust cython
                  dependency checking.
    --system      install to /usr
    --home        install to $HOME/usr
    --prefix=XXX  install to XXX

EOF
}

die() {
    echo "ERROR: $*"
    exit 1
}

PREFIX=""
for arg in $@; do
    case $arg in
        --force)
            touch $C_FILES
            ;;
        --system)
            PREFIX="/usr"
            ;;
        --home)
            PREFIX="$HOME/usr"
            ;;
        --help)
            usage
            exit 0
            ;;
        --prefix=*)
            PREFIX=`echo "$arg" | cut -d= -f2`
            ;;
        *)
            die "unknown option $arg"
            ;;
    esac
done

if [ -z "$PREFIX" ]; then
    if [ ! -z "$PYTHONPATH" ] && \
        echo "$PYTHONPATH" | tr ':' '\n' | grep -e "^$HOME/usr" >/dev/null 2>/dev/null; then
        PREFIX="$HOME/usr"
    else
        PREFIX="/usr"
    fi

    echo "INFO: no --system, --home or explicit prefix given, using $PREFIX"
fi

PYINCDIR=`python -c "import distutils.sysconfig; print distutils.sysconfig.get_python_inc(prefix='$PREFIX')"`
PYLIBDIR=`python -c "import distutils.sysconfig; print distutils.sysconfig.get_python_lib(prefix='$PREFIX')"`

if ! python -c "import sys; sys.exit([-1, 0][('$PYLIBDIR' in sys.path)])"; then
    die "library dir $PYLIBDIR not in \$PYTHONPATH or any known sys.path"
fi
if [ ! -d "$PREFIX" ]; then
    die "prefix $PREFIX is not a directory"
fi
if ! touch "$PREFIX" 2>/dev/null; then
    die "no permission to write to $PREFIX. needs sudo?"
fi
if [ ! -d "PYINCDIR" ] && ! mkdir -p $PYINCDIR 2>/dev/null; then
    die "could not create $PYINCDIR"
fi
if [ ! -d "PYLIBDIR" ] && ! mkdir -p $PYLIBDIR 2>/dev/null; then
    die "could not create $PYLIBDIR"
fi

if [ -z "$TMPDIR" ]; then
    TMPDIR="/tmp"
fi
DNAME=`dirname $0`
if [ "$DNAME" = "." ]; then
    DNAME="$PWD"
fi
PRJ=`basename $DNAME`
LOG="$TMPDIR/$PRJ"
LOG_OUT="$LOG.out"
LOG_ERR="$LOG.err"

echo ""
echo "install: $PRJ prefix=$PREFIX"
python setup.py build_ext --include-dirs="$PYINCDIR" develop --prefix="$PREFIX" install_headers --install-dir="$PYINCDIR" >"$LOG_OUT" 2>"$LOG_ERR"
if [ $? -eq 0 ]; then
    echo "successfully installed, see $LOG_OUT and $LOG_ERR"
else
    echo "failed to install, see $LOG_OUT and $LOG_ERR"
fi

