## Process this file with automake to produce Makefile.in

INCLUDES = \
	-Wall -I../lib \
	@evas_cflags@ \
	@ecore_cflags@ \
	@edje_cflags@ \
	@pbb_cflags@

bin_PROGRAMS = epbbuttons

epbbuttons_SOURCES = \
epbb.c epbb.h \
main.c

epbbuttons_LDADD = \
@evas_libs@ \
@ecore_libs@ \
@edje_libs@ \
@pbb_libs@ \
-lm -lc
