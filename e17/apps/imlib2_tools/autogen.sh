#!/bin/sh
# /bin/sh is a symlink to /bin/bash on the linux systems,
# but bash is not installed on SunOS/Solaris < 9, therefore I keep /bin/sh

# Run this to generate all the initial makefiles, etc.

abort () {
    echo "$1 not found or command failed. Aborting!"
    exit 1
}

if [ "$USER" = "root" ]; then
  echo "You cannot do this as "$USER" please use a normal user account"
  exit
fi

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

THEDIR=`pwd`
cd $srcdir

DIE=0

(autoconf --version) < /dev/null > /dev/null 2>&1 || {
	echo
	echo "You must have autoconf installed to compile imlib2_convert."
	echo "Download the appropriate package for your distribution,"
	echo "or get the source tarball at ftp://ftp.gnu.org/pub/gnu/"
	DIE=1
}

(automake --version) < /dev/null > /dev/null 2>&1 || {
	echo
	echo "You must have automake installed to compile imlib2_convert."
	echo "Get ftp://ftp.cygnus.com/pub/home/tromey/automake-1.2d.tar.gz"
	echo "(or a newer version if it is available)"
	DIE=1
}

if test "$DIE" -eq 1; then
	exit 1
fi

if test -z "$*"; then
	echo "I am going to run ./configure with no arguments - if you wish "
        echo "to pass any to it, please specify them on the $0 command line."
fi

case $CC in
xlc )
    am_opt=--include-deps;;
esac

aclocal $ACLOCAL_FLAGS || abort "aclocal"
(autoheader --version)  < /dev/null > /dev/null 2>&1 && ( 
 autoheader || abort "autoheader"
) 
automake --add-missing $am_opt || abort "automake"
autoconf || abort "autoconf"
cd $THEDIR

$srcdir/configure "$@" || abort "configure"

echo 
echo "Now type 'make' to compile imlib2_convert."

