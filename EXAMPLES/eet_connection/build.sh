#!/bin/sh

colorgcc -W -Wall -O0 -g -ggdb3 -c server.c -o server.o `pkg-config --libs --cflags eina ecore ecore-con eet`
colorgcc -W -Wall -O0 -g -ggdb3 -c client.c -o client.o `pkg-config --libs --cflags eina ecore ecore-con eet`
colorgcc -W -Wall -O0 -g -ggdb3 -c serialisation.c -o serialisation.o `pkg-config --libs --cflags eina ecore ecore-con eet`

colorgcc -W -Wall -O0 -g -ggdb3 -o tailor client.o serialisation.o `pkg-config --libs --cflags eina ecore ecore-con eet`
colorgcc -W -Wall -O0 -g -ggdb3 -o stock server.o serialisation.o `pkg-config --libs --cflags eina ecore ecore-con eet`

