#!/bin/sh

DIRNAME=`basename $PWD`

find . -name Makefile -delete
find . -name Makefile.in -delete

autoreconf -f -i

if [ -z "$NOCONFIGURE" ]; then
	./configure -C "$@"
fi

