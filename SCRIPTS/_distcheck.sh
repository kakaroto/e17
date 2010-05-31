#!/bin/sh

set -e

PROJ=$1
cd $PROJ
./autogen.sh || true
make clean distclean || true
./autogen.sh || true
make clean maintainer-clean || true
./autogen.sh
make distcheck
rm -f *.bz2 *.gz || true
make clean maintainer-clean || true
exit 0
