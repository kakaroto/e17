
## Process this file with automake to produce Makefile.in

SUBDIRS = src data

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess \
		   config.h.in config.sub configure install-sh \
		   ltconfig ltmain.sh missing mkinstalldirs \
		   stamp-h.in debian/changelog

bin_SCRIPTS = ewl-config

EXTRA_DIST = README AUTHORS COPYING ewl.m4 ewl.spec ewl.pc.in \
	     debian/changelog \
	     debian/rules debian/control debian/copyright \
	     debian/libewl0.files debian/libewl0-dev.files \
	     debian/ewl0-bin.files

m4datadir = $(datadir)/aclocal
m4data_DATA = ewl.m4

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = ewl.pc
