## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = 1.4 foreign

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = Makefile.in				

LDFLAGS              = -L/usr/X11R6/lib
INCLUDES             = -I. -I$(top_srcdir) -I$(top_srcdir)/src \
		       -I$(top_srcdir)/loaders -I$(top_srcdir)/libltdl \
                       -I/usr/X11R6/include $(X_CFLAGS) \
		       -I$(prefix)/include -I$(includedir)

if HAVE_X

bin_PROGRAMS         = imlib2 bumpmaptest imlib2_test polytest color_spaces
imlib2_SOURCES       = main.c
imlib2_LDADD         = $(top_builddir)/src/libImlib2.la

bumpmaptest_SOURCES  = bmtest.c
bumpmaptest_LDADD    = $(top_builddir)/src/libImlib2.la

imlib2_test_SOURCES  = imlib2_test.c
imlib2_test_LDADD    = $(top_builddir)/src/libImlib2.la

polytest_SOURCES     = polytest.c
polytest_LDADD       = $(top_builddir)/src/libImlib2.la

color_spaces_SOURCES = color_spaces.c
color_spaces_LDADD   = $(top_builddir)/src/libImlib2.la

endif

EXTRA_DIST = \
test_images/audio.png \
test_images/bg.png \
test_images/bulb.png \
test_images/cal.png \
test_images/calc.png \
test_images/folder.png \
test_images/globe.png \
test_images/imlib2.png \
test_images/lock.png \
test_images/mail.png \
test_images/menu.png \
test_images/mush.png \
test_images/paper.png \
test_images/sh1.png \
test_images/sh2.png \
test_images/sh3.png \
test_images/stop.png \
test_images/tnt.png \
ttfonts/cinema.ttf \
ttfonts/grunge.ttf \
ttfonts/morpheus.ttf \
ttfonts/notepad.ttf \
bmtest.c \
color_spaces.c \
imlib2_test.c \
main.c \
polytest.c
