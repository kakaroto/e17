#!/bin/bash

CF="-DHAVE_LIMITS_H -DSTDC_HEADERS -DHAVE_MEMCPY=1 -D_GNU_SOURCE=1 -O0 -pipe -Wall -Wextra -g"

DEPS=($(pkg-config --print-requires-private ecore-con))
echo "DEPENDENCIES: ${DEPS[@]}"
CFLAGS="$(pkg-config --static --cflags ${DEPS[@]} ecore-con)"
#CFLAGS+=" -I/usr/include/sasl"
echo "DEPENDENCY CFLAGS: $CFLAGS"

LIBS="$(pkg-config --static --libs ${DEPS[@]} ecore-con elementary)"
if (echo "$LIBS" | grep gnutls &> /dev/null) ; then
	LIBS+=" $(pkg-config --static --libs gnutls)"
fi
#LIBS+=" -lsasl2"
echo "DEPENDENCY LIBS: $LIBS"
echo

SOURCES=(*.c)

for x in ${SOURCES[@]};do
	echo "gcc -c $x -o ${x/.c/.o} $CFLAGS $CF || exit 1"
	gcc -c $x -o ${x/.c/.o} $CFLAGS $CF || exit 1
done

gcc *.o -o email -L/usr/lib -lc $LIBS
