#!/bin/bash

CF="-DHAVE_LIMITS_H -DSTDC_HEADERS -DHAVE_MEMCPY=1 -D_GNU_SOURCE=1 -O0 -pipe -Wall -Wextra -g"

DEPS=($(pkg-config --print-requires-private ecore-con))
echo "DEPENDENCIES: ${DEPS[@]}"
CFLAGS="$(pkg-config --static --cflags ${DEPS[@]} ecore-con)"
#CFLAGS+=" -I/usr/include/sasl"
echo "DEPENDENCY CFLAGS: $CFLAGS"

LIBS="$(pkg-config --static --libs ${DEPS[@]} ecore-con)"
if (echo "$LIBS" | grep gnutls &> /dev/null) ; then
	LIBS+=" $(pkg-config --static --libs gnutls)"
fi
#LIBS+=" -lsasl2"
echo "DEPENDENCY LIBS: $LIBS"
echo

SOURCES=(*.c)

for x in ${SOURCES[@]};do
	[[ "${x/test_}" != "$x" ]] && continue
	echo "gcc -c $x -o ${x/.c/.o} $CFLAGS $CF || exit 1"
	gcc -c $x -o ${x/.c/.o} $CFLAGS $CF || exit 1
done

gcc test_pop3.c *.o -o test_pop3 $CFLAGS $CF -L/usr/lib -lc $LIBS
gcc test_smtp.c *.o -o test_smtp $CFLAGS $CF -L/usr/lib -lc $LIBS
