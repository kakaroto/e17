## Process this file with automake to produce Makefile.in

INCLUDES = \
	-I$(top_srcdir)/intl \
	@GTK_CFLAGS@ @evas_cflags@ @edb_cflags@

bin_PROGRAMS = etcher

etcher_SOURCES = \
	etcher.c etcher.h \
	support.c support.h \
	interface.c interface.h \
	callbacks.c callbacks.h \
	bits.c bits.h \
	preferences.c preferences.h \
	splash.c splash.h \
	recent.c recent.h \
	globals.h

etcher_LDADD = @evas_libs@ @edb_libs@ -lm @GTK_LIBS@ $(INTLLIBS)

