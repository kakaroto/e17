#!/bin/sh

## Script to bootstrap the build enviroment

aclocal $ACLOCAL_FLAGS -I m4
autoconf
autoheader
automake --add-missing --copy --gnu
