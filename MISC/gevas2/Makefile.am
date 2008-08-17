
## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = dist-bzip2 subdir-objects
#AUTOMAKE_OPTIONS     = 1.4 foreign

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = INSTALL Makefile.in aclocal.m4 config.guess \
		       config.h.in config.h config.sub configure install-sh \
		       ltconfig ltmain.sh missing mkinstalldirs \
		       stamp-h.in

SUBDIRS		     = src media demo

EXTRA_DIST = README AUTHORS COPYING TODO acconfig.h INSTALL gevas.spec gevas.pc.in gevas.pc

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = gevas.pc
