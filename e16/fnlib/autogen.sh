#!/bin/sh

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.
THEDIR="`pwd`"
cd "$srcdir"
libtoolize --copy --force
aclocal $ACLOCAL_FLAGS
automake --add-missing
autoconf
cd "$THEDIR"
$srcdir/configure $@
