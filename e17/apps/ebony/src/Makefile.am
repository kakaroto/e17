## Process this file with automake to produce Makefile.in

INCLUDES = \
	-I$(top_srcdir)/intl \
	@BONUS_INCS@ \
	@GTK_CFLAGS@

bin_PROGRAMS = ebony

ebony_SOURCES = \
	main.c \
	support.c support.h \
	interface.c interface.h \
	callbacks.c callbacks.h \
	utils.c utils.h

ebony_LDADD = \
@GTK_LIBS@ $(INTLLIBS) \
@BONUS_LIBS@

