## Process this file with automake to produce Makefile.in

SUBDIRS = src test data

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess esmart-config.h.in \
			config.sub configure install-sh ltconfig ltmain.sh \
			missing mkinstalldirs stamp-h.in config.h.in depcomp

bin_SCRIPTS = esmart-config


EXTRA_DIST = README AUTHORS COPYING esmart.spec esmart.pc.in

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = esmart.pc

