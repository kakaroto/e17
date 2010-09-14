#!/bin/sh

usage() {
    cat <<EOF
Usage:

    $0 <options> [packages]

where options:

    --pypi        upload packages to Python Package Index.
    --system      install to /usr
    --home        install to $HOME/usr
    --prefix=XXX  install to XXX

NOTE: In order to build the packages you need Cython and a compiler
      installed, with python-efl working, as it is required to call
      setuptool's build_ext phase to generate the .c files from .pyx.

EOF
}

die() {
    echo "ERROR: $*"
    exit 1
}

PREFIX=""
UPLOAD=""
PACKAGES=""
for arg in $@; do
    case $arg in
        --pypi)
            UPLOAD="upload"
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
            PACKAGES="${PACKAGES} $arg"
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

if ! touch "$PREFIX" 2>/dev/null; then
    die "no permission to write to $PREFIX. needs sudo?"
fi
if [ ! -d "PYINCDIR" ] && ! mkdir -p $PYINCDIR 2>/dev/null; then
    die "could not create $PYINCDIR"
fi
if [ ! -d "PYLIBDIR" ] && ! mkdir -p $PYLIBDIR 2>/dev/null; then
    die "could not create $PYLIBDIR"
fi
if ! which cython >/dev/null; then
    die "cython is not installed"
fi

if [ -z "$PACKAGES" ]; then
    PACKAGES="python-evas python-ecore python-edje python-emotion python-e_dbus python-ethumb python-elementary python-efl_utils"
fi

for p in $PACKAGES; do
    if [ ! -d "$p" ]; then
        die "Not a valid package directory: $p"
    elif [ ! -f "$p/setup.py" ]; then
        die "Not a valid python setup (misses $p/setup.py)"
    fi
done

if [ -z "$TMPDIR" ]; then
    TMPDIR="/tmp"
fi

echo "Install to $PREFIX"
CWD=$PWD
for p in $PACKAGES; do
    echo "$p"
    cd $CWD/$p || die "cd $p"

    PRJ=`basename $p`
    LOG="$TMPDIR/$PRJ"
    LOG_OUT="$LOG.out"
    LOG_ERR="$LOG.err"

    TOUCH_FILES=`grep '[.]pyx' setup.py | sed "s/.*[\"']\([^\"']\+\)[.]pyx[\"'].*/\1/g"`
    for f in $TOUCH_FILES; do
        rm -f "$f.c"
        if [ -f "$f.pyx" ]; then
            touch "$f.pyx"
        else
            die "Source file does not exist: $f.pyx"
        fi
    done

    python setup.py \
        build_ext --include-dirs="$PYINCDIR" \
        install --prefix="$PREFIX" \
        install_headers --install-dir="$PYINCDIR" \
        sdist $UPLOAD >"$LOG_OUT" 2>"$LOG_ERR" \
        || die "Failed to package $p, see $LOG_OUT and $LOG_ERR"
    echo "$p done, see $LOG_OUT and $LOG_ERR"
done
