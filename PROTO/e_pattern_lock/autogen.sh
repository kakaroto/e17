#!/bin/sh

rm -rf autom4te.cache
rm -f aclocal.m4 ltmain.sh

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

ORIGDIR=`pwd`
cd $srcdir

autoreconf -v --install || exit 1
#intltoolize -c --automake --force || exit 1
cd $ORIGDIR || exit $?

if [ -z "$NOCONFIGURE" ]; then
	$srcdir/configure "$@"
fi
