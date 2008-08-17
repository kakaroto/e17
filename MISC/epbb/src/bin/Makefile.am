## Process this file with automake to produce Makefile.in

INCLUDES = \
	-Wall -I../lib \
	@EVAS_CFLAGS@ \
	@ECORE_CFLAGS@ \
	@EDJE_CFLAGS@ \
	@pbb_cflags@

bin_PROGRAMS = epbb

epbb_SOURCES = \
epbb.c epbb.h \
main.c

epbb_LDADD = \
@EVAS_LIBS@ \
@ECORE_LIBS@ \
@EDJE_LIBS@ \
@pbb_libs@ \
-lm
