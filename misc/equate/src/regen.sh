#!/bin/sh -e
flex calc.l
bison calc.y
#gcc -o calc.o calc.tab.c -c
