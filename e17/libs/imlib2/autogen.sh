#! /bin/sh

if [ "$USER" = "root" ]; then
  echo "You cannot do this as "$USER" please use a normal user account"
  exit
fi

abort () {
    echo "$1 not found or command failed. Aborting!"
    exit 1
}

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

#SUBDIRS=". libltdl"
SUBDIRS=.

set -x

for EACHDIR in $SUBDIRS ; do
    cd $THEDIR/$EACHDIR
    aclocal || abort "aclocal"
    libtoolize --ltdl --force --copy || abort "libtoolize"
    autoheader || abort "autoheader"
    automake --foreign --add-missing || abort "automake"
    autoconf || abort "autoconf"
    (cd libltdl && aclocal) || abort "in libltdl, aclocal"
    (cd libltdl && autoheader) || abort "in libltdl, autoheader"
    (cd libltdl && automake --add-missing) || abort "in libltdl, automake"
    (cd libltdl && autoconf) || abort "in libltdl, autoconf"
done

if test -z "$*"; then
        echo "I am going to run ./configure with no arguments - if you wish "
        echo "to pass any to it, please specify them on the $0 command line."
fi

cd "$THEDIR"

$srcdir/configure "$@" || abort "configure"

set +x

echo "Now type:"
echo
echo "make"
echo "make install"
echo
echo "have fun."

