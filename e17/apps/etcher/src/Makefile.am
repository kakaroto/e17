## Process this file with automake to produce Makefile.in

INCLUDES = \
	-I$(top_srcdir)/intl \
	@GTK_CFLAGS@ @evas_cflags@

bin_PROGRAMS = etcher

etcher_SOURCES = \
	main.c \
	support.c support.h \
	interface.c interface.h \
	callbacks.c callbacks.h \
	bits.c bits.h

etcher_LDADD = @evas_libs@ @GTK_LIBS@ $(INTLLIBS)

