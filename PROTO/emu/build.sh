#!/bin/bash

VREV="$(git log -n1 --pretty=oneline 2> /dev/null | cut -d' ' -f1 | tr -d '\n')"

CF="-DHAVE_LIMITS_H -DSTDC_HEADERS -DHAVE_MEMCPY=1 -DVREV=\"$VREV\" -I$(readlink -f .) -D_GNU_SOURCE=1 -O0 -pipe -Wall -Wextra -g -I$(readlink -f src/include) -DHAVE_ECORE_X -DHAVE_DBUS -DHAVE_NOTIFY -DPACKAGE_DATA_DIR=\".\""
[[ -z "$CC" ]] && CC=gcc
[[ -z "$CXX" ]] && CXX=g++
#DEPS=($(pkg-config --print-requires-private ecore-con edbus ecore-x elementary enotify))
#echo "DEPENDENCIES: ${DEPS[@]}"
CFLAGS="$(pkg-config --cflags eina eet evas edje ecore ecore-con)"
#echo "DEPENDENCY CFLAGS: $CFLAGS"
LIBS="$(pkg-config --libs eina eet evas edje ecore ecore-con edbus ecore-x)"
CFLAGS+=" $(pkg-config --cflags azy) -DHAVE_AZY"
LIBS+=" $(pkg-config --libs azy)"
#echo "DEPENDENCY LIBS: $LIBS"
#echo

link=0
compile=0

#edje_cc -id data/theme data/theme/default.edc default.edj || exit 1

if [[ -f ./emu ]] ; then
	for x in *.azy *.h ; do # src/{bin,include,lib}/*.h ; do
		if [[ "$x" -nt ./emu ]] ; then
			compile=1
			break;
		fi
	done
fi


for x in *.azy ; do
	azy_parser -Hn $x
done

for x in *.c ; do
	[[ $compile == 0 && -f "${x/.c/.o}" && "$x" -ot "${x/.c/.o}" ]] && continue
#	echo "$CC -c $x -o ${x/.c/.o} $CFLAGS $CF || exit 1"
	echo "$CC $x"
	($CC -c $x -o "${x/.c/.o}" $CFLAGS $CF || exit 1)&
	link=1
done

[[ $link == 0 ]] && exit 1
wait
#echo "$CXX *.o -o emu -L/usr/lib -lc $LIBS"
echo "$CXX *.o -o emu"
$CXX *.o -o emu -L/usr/lib -lc $LIBS
