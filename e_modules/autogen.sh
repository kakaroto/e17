#!/bin/sh

for d in * ; do
	[ ! -x "${d}"/autogen.sh ] && continue
	echo "./$d/autogen.sh"
	cd $d
	./autogen.sh "$@" || exit 1
	cd ..
done

cp configure.in configure
chmod a+rx configure
