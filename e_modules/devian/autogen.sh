#!/bin/sh

rm -Rf  aclocal.m4 autom4te.cache/ ltmain.sh config.log config.status config.cache configure configure.scan libtool Makefile \
	data/Makefile.in data/Makefile \
	data/images/Makefile.in data/images/Makefile \
	data/themes/Makefile.in data/themes/Makefile \
	data/themes/simple/Makefile.in data/themes/simple/Makefile \
	data/themes/clean/Makefile.in data/themes/clean/Makefile \
	src/Makefile src/Makefile.in \
	src/module/Makefile src/module/Makefile.in

touch README

echo
echo "Options : (see INSTALL file)"
echo "   No arguments : Build dEvian module (Picture & Rss feature)"
echo "   --enable-modular=rss : Build dErss module (Rss feature only)"
echo "   --enable-modular=log : Build dElog module (Log feature only)"
echo
echo "Running aclocal..." ; aclocal $ACLOCAL_FLAGS -I m4 || exit 1
echo "Running autoheader..." ; autoheader || exit 1
echo "Running autoconf..." ; autoconf || exit 1
echo "Running libtoolize..." ; (libtoolize --copy --automake || glibtoolize --automake) || exit 1
echo "Running automake..." ; automake --add-missing --copy --gnu || exit 1

if [ -z "$NOCONFIGURE" ]; then
	./configure "$@"
fi
