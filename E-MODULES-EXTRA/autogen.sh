#!/bin/sh

rm -rf autom4te.cache
rm -f aclocal.m4 ltmain.sh

for d in * ; do
	test -d "$d" || continue
	sed "s,%MODNAME%,$d,g" e_modules-TEMPLATE.spec.in > "$d/e_modules-$d.spec.in"
	test -x "${d}/autogen.sh" || continue
	echo "AUTOGEN:  $d (no output = Makefile of $d exists, no need to run autogen)"
	test -f "${d}/Makefile" || (cd $d && ./autogen.sh "$@") || exit 1
done

cp -p configure.ac configure
cp -p Makefile.in Makefile
chmod a+rx configure
