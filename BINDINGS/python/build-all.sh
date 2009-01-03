#!/bin/sh

PREFIX=$1
[ -z "$PREFIX" ] && PREFIX="$HOME/usr"

die() {
    echo "ERROR: $@"
    exit 1
}

echo "Install to $PREFIX"
CWD=$PWD
for m in evas ecore edje emotion e_dbus epsilon etk elementary; do
    cd $CWD/python-$m || die "cd python-$m"
    ./build.sh --force $PREFIX || die "failed to build python-$m"
done
