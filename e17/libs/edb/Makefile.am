
## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS     = 1.4 foreign

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = INSTALL Makefile.in aclocal.m4 config.guess \
		       config.h.in config.sub configure install-sh \
		       ltconfig ltmain.sh missing mkinstalldirs \
		       stamp-h.in

SUBDIRS		     = src test tools debian

bin_SCRIPTS = edb-config

EXTRA_DIST = README AUTHORS COPYING edb.spec edb.pc.in

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = edb.pc
