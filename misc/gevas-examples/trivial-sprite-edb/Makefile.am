## Process this file with automake to produce Makefile.in

INCLUDES = \
	-I$(top_srcdir)/intl \
	@GTK_CFLAGS@ @evas_cflags@ @edb_cflags@ \
	-I/usr/local/include

FONTS = andover.ttf

EXTRA_DIST = $(FONTS) data.db
pkgdata_DATA = $(FONTS) data.db

#TESTS = gevasanim

bin_PROGRAMS = trivial-sprite-edb

trivial_sprite_edb_SOURCES = trivial-sprite-edb.c

INCLUDES = -I$(srcdir) -I../src -I.. -I. -I$(topdir)/src \
	@GTK_CFLAGS@ @evas_cflags@

trivial_sprite_edb_LDADD =  @evas_libs@ -lm @GTK_LIBS@ \
	@edb_libs@ -lgevas



