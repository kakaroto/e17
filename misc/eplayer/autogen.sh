#!/bin/sh

## Script to bootstrap the build enviroment

rm -rf autom4te.cache
rm -f aclocal.m4

aclocal $ACLOCAL_FLAGS -I m4
autoheader
autoconf
libtoolize --automake
automake --add-missing --copy --gnu
