
## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS     = 1.4 foreign

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = INSTALL Makefile.in aclocal.m4 config.guess \
		       config.h.in config.sub configure install-sh \
		       ltconfig ltmain.sh missing mkinstalldirs \
		       stamp-h.in

SUBDIRS		     = libltdl src filters doc loaders

bin_SCRIPTS = imlib2-config

EXTRA_DIST = \
imlib2.spec \
imlib2.pc.in \
demo/Makefile.am \
demo/Makefile.in \
demo/view.c \
demo/imconvert.c \
test/Makefile.am \
test/Makefile.in \
test/main.c \
test/test_images/cal.png \
test/test_images/lock.png \
test/test_images/paper.png \
test/test_images/stop.png \
test/test_images/audio.png \
test/test_images/calc.png \
test/test_images/mail.png \
test/test_images/sh1.png \
test/test_images/tnt.png \
test/test_images/bg.png \
test/test_images/folder.png \
test/test_images/menu.png \
test/test_images/sh2.png \
test/test_images/bulb.png \
test/test_images/globe.png \
test/test_images/mush.png \
test/test_images/sh3.png \
test/ttfonts/cinema.ttf \
test/ttfonts/grunge.ttf \
test/ttfonts/morpheus.ttf \
test/ttfonts/notepad.ttf

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = imlib2.pc

