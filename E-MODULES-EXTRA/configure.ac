#!/bin/sh

for d in * ; do
        if test "x$d" = "xscreenshot"; then continue ; fi
	test -d "$d" || continue
	test -x "${d}/configure" || continue
	echo "CONFIGURE:  $d (no output = Makefile of $d exists, no need to run autogen)"
	test -f "${d}/Makefile" || (cd $d && ./configure "$@") || exit $?
done

cp -p Makefile.in Makefile
