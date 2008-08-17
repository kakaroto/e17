#!/bin/sh

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

THEDIR="`pwd`"
cd "$srcdir"
DIE=0

aclocal
autoconf
autoheader
automake --foreign --add-missing

if [ -z "$NOCONFIGURE" ]; then

if test -z "$*"; then
        echo "I am going to run ./configure with no arguments - if you wish "
        echo "to pass any to it, please specify them on the $0 command line."
fi

cd "$THEDIR"

$srcdir/configure "$@"

mpg123 ./feh.mp3 >/dev/null 2>&1 &

fi
