#! /bin/sh

set -x
autoheader
libtoolize --ltdl --force --copy
aclocal
automake --foreign --add-missing --copy
autoconf

