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

echo "Running autopoint..." ; autopoint -f || :
echo "Running aclocal..." ; aclocal $ACLOCAL_FLAGS -I m4 || exit 1
echo "Running autoheader..." ; autoheader || exit 1
echo "Running autoconf..." ; autoconf || exit 1
echo "Running libtoolize..." ; (libtoolize --copy --automake || glibtoolize --automake) || exit 1
echo "Running automake..." ; automake --add-missing --copy --gnu || exit 1

cd $ORIGDIR || exit 1

if [ -z "$NOCONFIGURE" ]; then
    $srcdir/configure $AUTOGEN_CONFIGURE_ARGS "$@" || exit $?
fi
