## Process this file with automake to produce Makefile.in

INCLUDES = \
	-I$(top_srcdir)/intl \
	@GTK_CFLAGS@ @evas_cflags@ @edb_cflags@ \
	-I/usr/local/include

PIXMAPS = \
cell1.png \
cell2.png \
cell3.png \
cell4.png \
cell5.png \
cell6.png \
bg.png

FONTS = \
andover.ttf

EXTRA_DIST = $(PIXMAPS) $(FONTS) cell.db
pkgdata_DATA = $(PIXMAPS) $(FONTS) cell.db

#TESTS = gevasanim

bin_PROGRAMS = gevasanim

gevasanim_SOURCES = \
	gevasanim.c

INCLUDES = -I$(srcdir) -I../src -I.. -I. -I$(topdir)/src \
	@GTK_CFLAGS@ @evas_cflags@

gevasanim_LDADD =  @evas_libs@ -lm @GTK_LIBS@ \
	@edb_libs@  \
	-lgevas
	#/usr/local/lib/libgevas.a



