#!/bin/sh

for d in * ; do
	test -d "$d" || continue
	sed "s,%MODNAME%,$d,g" e_modules-TEMPLATE.spec.in > "$d/e_modules-$d.spec.in"
	test -x "${d}/autogen.sh" || continue
	echo "AUTOGEN:  $d"
	(cd $d && ./autogen.sh "$@") || exit 1
done

cp -p configure.ac configure
cp -p Makefile.in Makefile
chmod a+rx configure
