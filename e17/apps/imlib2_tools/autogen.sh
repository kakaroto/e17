#! /bin/sh

if [ "$USER" = "root" ]; then
  echo "You cannot do this as "$USER" please use a normal user account"
  exit
fi

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

THEDIR="`pwd`"
cd "$srcdir"
DIE=0

set -x
aclocal
libtoolize --ltdl --force --copy
autoconf
autoheader
automake --foreign --add-missing

if test -z "$*"; then
        echo "I am going to run ./configure with no arguments - if you wish "
        echo "to pass any to it, please specify them on the $0 command line."
fi

cd "$THEDIR"

$srcdir/configure "$@"

set +x

echo "Now type:"
echo
echo "make"
echo "make install"
echo
echo "have fun."

