#!/bin/bash

CF="-DHAVE_LIMITS_H -DSTDC_HEADERS -DHAVE_MEMCPY=1 -D_GNU_SOURCE=1 -O0 -pipe -Wall -Wextra -g"

DEPS=($(pkg-config --print-requires-private elementary))
echo "DEPENDENCIES: ${DEPS[@]}"
CFLAGS="$(pkg-config --static --cflags ${DEPS[@]} elementary)"
echo "DEPENDENCY CFLAGS: $CFLAGS"

LIBS="$(pkg-config --static --libs ${DEPS[@]} elementary)"
echo "DEPENDENCY LIBS: $LIBS"
echo

SOURCES=(*.c)

for x in ${SOURCES[@]};do
	[[ "${x/test_}" != "$x" ]] && continue
	echo "gcc -c $x -o ${x/.c/.o} $CFLAGS $CF || exit 1"
	gcc -c $x -o ${x/.c/.o} $CFLAGS $CF || exit 1
done

gcc *.o -o ev $CFLAGS $CF -L/usr/lib -lc $LIBS
