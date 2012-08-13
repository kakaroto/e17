#!/bin/bash

VREV="$(git log -n1 --pretty=oneline 2> /dev/null | cut -d' ' -f1 | tr -d '\n')"

CF="-DHAVE_LIMITS_H -DSTDC_HEADERS -DHAVE_MEMCPY=1 -DDATA_DIR=\"/usr/share\" -DPACKAGE_SRC_DIR=\"$(dirname $(readlink -f $0))\" -DVREV=\"$VREV\" -I$(readlink -f .) -D_GNU_SOURCE=1 -O0 -pipe -Wall -Wextra -g -I$(readlink -f src/include) -DHAVE_DBUS -DHAVE_NOTIFY -DPACKAGE_DATA_DIR=\".\""
[[ -z "$CC" ]] && CC=gcc
[[ -z "$CXX" ]] && CXX=g++
#DEPS=($(pkg-config --print-requires-private ecore-con edbus elementary enotify))
#echo "DEPENDENCIES: ${DEPS[@]}"
CFLAGS="$(pkg-config --cflags eina eet evas edje ecore ecore-con edbus elementary enotify ecore-file efx)"
#echo "DEPENDENCY CFLAGS: $CFLAGS"
LIBS="$(pkg-config --libs eina eet evas edje ecore ecore-con edbus elementary enotify ecore-file efx)"
if pkg-config --exists azy ; then
	CFLAGS+=" $(pkg-config --cflags azy) -DHAVE_AZY"
	LIBS+=" $(pkg-config --libs azy)"
fi
#echo "DEPENDENCY LIBS: $LIBS"
#echo

link=0
compile=0

edje_cc -id data/theme data/theme/default.edc default.edj || exit 1

if [[ -f ./shotgun ]] ; then
	for x in *.h src/{bin,include,lib}/*.h ; do
		if [[ "$x" -nt ./shotgun ]] ; then
			compile=1
			break;
		fi
	done
fi

for x in src/lib/*.c src/bin/*.c  ; do
	[[ $compile == 0 && -f "${x/.c/.o}" && "$x" -ot "${x/.c/.o}" ]] && continue
#	echo "$CC -c $x -o ${x/.c/.o} $CFLAGS $CF || exit 1"
	echo "$CC $x"
	($CC -c $x -o "${x/.c/.o}" $CFLAGS $CF || exit 1)&
	link=1
done

for x in src/lib/*.cpp ; do
	[[ $compile == 0 && -f "${x/.cpp/.o}" && "$x" -ot "${x/.cpp/.o}" ]] && continue
#	echo "$CC -c $x -o ${x/.cpp/.o} $CFLAGS $CF || exit 1"
	echo "$CC $x"
	($CC -c $x -o "${x/.cpp/.o}" $CFLAGS $CF || exit 1)&
	link=1
done

[[ $link == 0 ]] && exit 1
wait
#echo "$CXX *.o -o shotgun -L/usr/lib -lc $LIBS" #pugixml.a
echo "$CXX *.o -o shotgun" #pugixml.a
$CXX src/lib/*.o src/bin/*.o -o shotgun -L/usr/lib -lc $LIBS #pugixml.a
