#!/bin/sh

## Script to bootstrap the build enviroment

aclocal
libtoolize --force --copy
autoconf
autoheader
automake --add-missing --copy --gnu
