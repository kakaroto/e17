## Process this file with automake to produce Makefile.in

INCLUDES = \
	@BONUS_INCS@ \
	@GTK_CFLAGS@

bin_PROGRAMS = ebindings

ebindings_SOURCES = \
	menus.c menus.h \
	viewer.c viewer.h

ebindings_LDADD = \
@GTK_LIBS@ \
@BONUS_LIBS@

