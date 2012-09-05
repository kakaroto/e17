#!/bin/sh

rm -rf autom4te.cache
rm -f aclocal.m4 ltmain.sh

DIRNAME=`basename $PWD`

find . -name Makefile -delete
find . -name Makefile.in -delete

autoreconf -f -i

if [ -z "$NOCONFIGURE" ]; then
	./configure "$@"
fi
