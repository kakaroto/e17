#!/bin/sh
# Run this to generate all the initial makefiles, etc.

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

THEDIR="`pwd`"
cd "$srcdir"
DIE=0

(autoconf --version) < /dev/null > /dev/null 2>&1 || {
        echo
        echo "You must have autoconf installed to compile Enlightenment."
        echo "Download the appropriate package for your distribution,"
        echo "or get the source tarball at ftp://ftp.gnu.org/pub/gnu/"
        DIE=1
}

(automake --version) < /dev/null > /dev/null 2>&1 || {
        echo
        echo "You must have automake installed to compile Enlightenment."
        echo "Get ftp://ftp.gnu.org/pub/gnu/automake-1.3.tar.gz"
        echo "(or a newer version if it is available)"
        DIE=1
}

if test "$DIE" -eq 1; then
        exit 1
fi

(test -d src && test -d dox) || {
        echo "You must run this script in the top-level Enlightenment directory"
        exit 1
}

if test -z "$*"; then
        echo "I am going to run ./configure with no arguments - if you wish "
        echo "to pass any to it, please specify them on the $0 command line."
fi

echo "Generating configuration files for Enlightenment, please wait...."

echo "  aclocal $ACLOCAL_FLAGS"
aclocal $ACLOCAL_FLAGS
echo "  autoheader"
autoheader
echo "  automake --add-missing"
automake --add-missing 
echo "  autoconf"
autoconf

cd "$THEDIR"

cd "$THEDIR/man"

cat ../dox/E-docs/MAIN | ./e2mp > enlightenment.pod
pod2man enlightenment.pod > enlightenment.1

cd "$THEDIR"

$srcdir/configure "$@" && echo && echo "Now type 'make' to compile Enlightenment."
