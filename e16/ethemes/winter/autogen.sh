#!/bin/sh
# Run this to generate all the initial makefiles, etc.

echo "  aclocal $ACLOCAL_FLAGS"
aclocal $ACLOCAL_FLAGS
echo "  automake --add-missing"
automake --add-missing 
echo "  autoconf"
autoconf

./configure "$@"
