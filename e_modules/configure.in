#!/bin/sh

for d in * ; do
	test -d "$d" || continue
	test -x "${d}/configure" || continue
	echo "CONFIGURE:  $d"
	(cd $d && ./configure "$@") || exit $?
done

cp -p Makefile.in Makefile
