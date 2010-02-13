#!/bin/sh

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
FORCE=""
for arg in $@; do
    case $arg in
        --force)
            FORCE="--force"
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

echo "Install to $PREFIX"
CWD=$PWD
for m in evas ecore edje emotion e_dbus ethumb elementary efl_utils; do
    cd $CWD/python-$m || die "cd python-$m"
    ./build.sh --prefix="$PREFIX" $FORCE || die "failed to build python-$m"
done
