#!/bin/sh
gcc -Wall -Wextra -fPIC -rdynamic -g3 -O0 -c libclouseau.c `pkg-config --libs --cflags elementary ecore evas`
gcc -g3 -O0 -shared -Wl,-soname,libclouseau.so.1 -o libclouseau.so.1.0.1 libclouseau.o -lc -ldl `pkg-config --libs --cflags elementary`

