#!/bin/sh

for d in * ; do
        if test "x$d" = "xscreenshot"; then continue ; fi
	test -d "$d" || continue
	test -x "${d}/configure" || continue
	echo "CONFIGURE:  $d"
	(cd $d && ./configure "$@") || exit $?
done

cp -p Makefile.in Makefile
