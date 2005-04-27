## Process this file with automake to produce Makefile.in

INCLUDES = \
	-I$(top_srcdir)/intl -I$(srcdir) -I$(top_srcdir)/src -I.. -I. \
	@GTK_CFLAGS@ @evas_cflags@ @edb_cflags@ \
	-I/usr/local/include

PIXMAPS = \
delete.png \
drawer_closed.png \
drawer_open.png \
drawer_open_socks.png \
raptor.png \
raptor2.jpg \
cd.png \
cell1.png \
cell2.png \
cell3.png \
cell4.png \
cell5.png \
cell6.png \
bg.png \
e_logo.png

FONTS = \
andover.ttf

EXTRA_DIST = $(PIXMAPS) $(FONTS)
pkgdata_DATA = $(PIXMAPS) $(FONTS)

TESTS = testgevas gwinmaptest imagesize rawimage gevasanim signalconnect
bin_PROGRAMS = $(TESTS)

testgevas_SOURCES = testgevas.c
testgevas_LDADD = @evas_libs@ ../src/libgevas.la -lm @GTK_LIBS@

gwinmaptest_SOURCES = gwinmaptest.c
gwinmaptest_LDADD = @evas_libs@ ../src/libgevas.la -lm @GTK_LIBS@

imagesize_SOURCES = imagesize.c
imagesize_LDADD = @evas_libs@ ../src/libgevas.la -lm @GTK_LIBS@

rawimage_SOURCES = rawimage.c
rawimage_LDADD = @evas_libs@ ../src/libgevas.la -lm @GTK_LIBS@

gevasanim_SOURCES = gevasanim.c
gevasanim_LDADD = @evas_libs@ ../src/libgevas.la -lm @GTK_LIBS@

signalconnect_SOURCES = signalconnect.c
signalconnect_LDADD = @evas_libs@ ../src/libgevas.la -lm @GTK_LIBS@


