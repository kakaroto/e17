#!/bin/sh

rm -rf autom4te.cache || echo "No cache"
rm -f aclocal.m4 ltmain.sh ||  echo "not run yet"

touch README

echo "Running aclocal..." ; aclocal || exit 1
echo "Running autoheader..." ; autoheader || exit 1
echo "Running autoconf..." ; autoconf || exit 1
echo "Running libtoolize..." ; (libtoolize --copy --automake || glibtoolize --automake) || exit 1
echo "Running automake..." ; automake --add-missing --copy --gnu || exit 1

if [ -z "$NOCONFIGURE" ]; then
	./configure "$@"
fi
