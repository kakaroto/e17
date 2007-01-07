#!/bin/sh
# Run this to generate all the initial makefiles, etc.

srcdir=`dirname $0`

(test -d src && test -d dox) || {
    echo "You must run this script in the top-level directory"
    exit 1
}

echo "Generating configuration files, please wait...."

rm -rf autom4te.cache
rm -f aclocal.m4

echo "Running autopoint -f"
autopoint -f || gautopoint -f || exit 1
rm -f ABOUT-NLS
echo "Running aclocal -I m4 $ACLOCAL_FLAGS"
aclocal -I m4 $ACLOCAL_FLAGS || exit 1
echo "Running autoconf"
autoconf || exit 1
echo "Running autoheader"
autoheader || exit 1
echo "Running libtoolize"
libtoolize --copy --automake || glibtoolize --automake || exit 1
echo "Running automake --add-missing"
automake --copy --add-missing || exit 1

if test x$NOCONFIGURE != x; then
    echo "Skipping configure process."
    exit
fi

#conf_flags="--enable-maintainer-mode --enable-compile-warnings"

if test -z "$*"; then
    echo "I am going to run ./configure with no arguments - if you wish "
    echo "to pass any to it, please specify them on the $0 command line."
fi

echo "Running $srcdir/configure $conf_flags $@ ..."
$srcdir/configure $conf_flags "$@"
