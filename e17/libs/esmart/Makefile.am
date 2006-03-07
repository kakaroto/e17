## Process this file with automake to produce Makefile.in

SUBDIRS = src data

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess esmart-config.h.in \
			config.sub configure install-sh ltconfig ltmain.sh \
			missing mkinstalldirs stamp-h.in config.h.in depcomp \
			debian/changelog esmart.spec

bin_SCRIPTS = esmart-config

EXTRA_DIST = README AUTHORS COPYING esmart.pc.in esmart.spec esmart.spec.in \
	     debian/control debian/copyright debian/rules \
	     debian/changelog \
	     debian/libesmart0-dev.files debian/esmart0-bin.files \
	     debian/libesmart0.files debian/esmart0-bin.files

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = esmart.pc

