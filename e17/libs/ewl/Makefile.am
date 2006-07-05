
## Process this file with automake to produce Makefile.in

SUBDIRS = src data

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess \
		   config.h.in config.sub configure install-sh \
		   ltconfig ltmain.sh missing mkinstalldirs \
		   stamp-h.in debian/changelog

bin_SCRIPTS = ewl-config

EXTRA_DIST = README AUTHORS COPYING ewl.m4 ewl.spec ewl.pc.in \
	     debian/rules \
	     debian/control \
	     debian/changelog \
	     debian/copyright \
	     debian/changelog.in \
	     debian/libewl0.install \
	     debian/ewl0-bin.install \
	     debian/libewl0-dev.install \
	     debian/libewl0-engine-x11.install \
	     debian/libewl0-engines-all.install \
	     debian/libewl0-engine-evas-fb.install \
	     debian/libewl0-engine-evas-gl-x11.install \
	     debian/libewl0-engine-evas-software-x11.install

m4datadir = $(datadir)/aclocal
m4data_DATA = ewl.m4

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = ewl.pc
