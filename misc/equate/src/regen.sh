#!/bin/sh -e
flex -ocalc_lex.c calc.l
bison calc.y -o calc_parse.c

