#!/bin/sh
# Run this to generate all the initial makefiles, etc.

rm -rf */install-sh */missing

echo "  aclocal $ACLOCAL_FLAGS"
aclocal $ACLOCAL_FLAGS
echo "  automake --add-missing"
automake --add-missing 
echo "  autoconf"
autoconf

./configure "$@"
