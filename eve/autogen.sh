#!/bin/sh

rm -rf autom4te.cache
rm -f aclocal.m4 ltmain.sh

DIRNAME=`basename $PWD`
if test "x$DIRNAME" = "xelementary-skel"; then
    echo "Do not run autogen.sh from inside elementary-skel"
    exit 1
fi

find . -name Makefile -delete
find . -name Makefile.in -delete

autoreconf -f -i

if [ -z "$NOCONFIGURE" ]; then
	./configure "$@"
fi
