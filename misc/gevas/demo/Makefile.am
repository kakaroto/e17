## Process this file with automake to produce Makefile.in

INCLUDES = \
	-I$(top_srcdir)/intl \
	@GTK_CFLAGS@ @evas_cflags@ @edb_cflags@ \
	-I/usr/local/include

PIXMAPS = \
delete.png \
drawer_closed.png \
drawer_open.png \
drawer_open_socks.png \
raptor.png \
raptor2.jpg \
cd.png

FONTS = \
andover.ttf

EXTRA_DIST = $(PIXMAPS) $(FONTS)
pkgdata_DATA = $(PIXMAPS) $(FONTS)

TESTS = testgevas

bin_PROGRAMS = testgevas

testgevas_SOURCES = \
	testgevas.c

INCLUDES = -I$(srcdir) -I../src -I.. -I. -I$(topdir)/src @gtk_cflags@

testgevas_LDADD = @evas_libs@ @edb_libs@  ../src/libgevas.la -lm @GTK_LIBS@ 

# $(INTLLIBS)

