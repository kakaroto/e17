## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = foreign

SUBDIRS = data src test

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess \
		   config.h.in config.sub configure install-sh \
		   ltconfig ltmain.sh missing mkinstalldirs \
		   stamp-h.in

bin_SCRIPTS = etox-config

EXTRA_DIST = README AUTHORS COPYING etox.m4 etox.spec

m4datadir = $(datadir)/aclocal
m4data_DATA = etox.m4

