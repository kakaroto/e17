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

e17genmenu_SOURCES = main.c global.c e_fdo_menu_to_order.c menus.c parse.c eaps.c order.c sort.c

noinst_HEADERS = global.h e_fdo_menu_to_order.h menus.h parse.h eaps.h order.h sort.h

e17genmenu_LDADD = @EET_LIBS@ @ECORE_LIBS@ @ENGRAVE_LIBS@
