#!/bin/bash

CF="-DSTATEDIR=\"/var\" -D_GNU_SOURCE=1 -O0 -pipe -Wall -Wextra -g"

DEPS=($(pkg-config --print-requires-private ecore-con))
echo "DEPENDENCIES: ${DEPS[@]}"
CFLAGS="$(pkg-config --static --cflags ${DEPS[@]} ecore-con)"
echo "DEPENDENCY CFLAGS: $CFLAGS"

LIBS="$(pkg-config --static --libs ${DEPS[@]} ecore-con)"
echo "DEPENDENCY LIBS: $LIBS"
echo

SOURCES=(*.c)

for x in ${SOURCES[@]};do
	echo "gcc -c $x -o ${x/.c/.o} $CFLAGS $CF || exit 1"
	gcc -c $x -o ${x/.c/.o} $CFLAGS $CF || exit 1
done

gcc *.o -o pa $CFLAGS $CF -L/usr/lib -lc $LIBS
