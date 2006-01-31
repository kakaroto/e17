## Process this file with automake to produce Makefile.in

INCLUDES = \
-I../../lib \
-I/usr/include \
-I/usr/local/include \
-I$(top_srcdir)/lib \
@EET_CFLAGS@ \
@ECORE_CFLAGS@ \
@ENGRAVE_CFLAGS@

AM_CFLAGS =\
	 -Wall\
	 -g

bin_PROGRAMS = e17genmenu

e17genmenu_SOURCES = main.c global.c menus.c parse.c eaps.c icons.c \
	category.c order.c sort.c

noinst_HEADERS = global.h menus.h parse.h eaps.h icons.h \
	category.h order.h sort.h

e17genmenu_LDADD = @EET_LIBS@ @ECORE_LIBS@ @ENGRAVE_LIBS@

