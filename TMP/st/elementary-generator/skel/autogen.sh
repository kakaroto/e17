#!/bin/sh

rm -rf autom4te.cache
rm -f aclocal.m4 ltmain.sh

# Allow invocation from a separate build directory; in that case, we change
# to the source directory to run the auto*, then change back before running configure
srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

ORIGDIR=`pwd`
cd $srcdir

DIRNAME=`basename $PWD`
if test "x$DIRNAME" = "xelementary-skel"; then
    echo "Do not run autogen.sh from inside elementary-skel"
    exit 1
fi

find . -name Makefile -delete
find . -name Makefile.in -delete

autoreconf -i

cd $ORIGDIR || exit 1

if [ -z "$NOCONFIGURE" ]; then
    $srcdir/configure $AUTOGEN_CONFIGURE_ARGS "$@" || exit $?
fi
