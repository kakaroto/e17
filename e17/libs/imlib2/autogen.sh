#! /bin/sh

(automake --version | grep 1.4) && (autoconf --version | grep 2.5)

if test $? = 1; then
  echo "Happy building ...";
else
  echo "Trouble!";
  echo "You have automake 1.4 and autoconf 2.5. either downgrade autoconf to";
  echo "2.13 or upgrade automake to 1.5";
  exit
fi

if [ "$USER" = "root" ]; then
  echo "You cannot do this as "$USER" please use a normal user account"
  exit
fi

echo
echo 
echo "NOTE:"
echo "you will need libtool 1.3 or higher for this to work"
echo
echo

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

THEDIR="`pwd`"
cd "$srcdir"
DIE=0

set -x
autoheader
libtoolize --ltdl --force --copy
aclocal
automake --foreign --add-missing
autoconf

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

