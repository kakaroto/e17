## Process this file with automake to produce Makefile.in

INCLUDES = \
	-I$(top_srcdir)/intl \
	@GTK_CFLAGS@ @evas_cflags@ @edb_cflags@ @imlib2_cflags@

bin_PROGRAMS = etcher

etcher_SOURCES = \
	bits.c bits.h \
	callbacks.c callbacks.h \
	etcher.c etcher.h \
	etching.c etching.h \
	file_dialog.c file_dialog.h \
	interface.c interface.h \
	preferences.c preferences.h \
	recent.c recent.h \
	splash.c splash.h \
	support.c support.h \
	workspace.c workspace.h \
	globals.h \
	macros.h \
	lights.h

etcher_LDADD = @evas_libs@ @edb_libs@ @imlib2_libs@ -lm @GTK_LIBS@ $(INTLLIBS)

