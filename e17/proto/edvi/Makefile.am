ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 compile \
		       config.guess config.h.in config.sub configure \
		       depcomp install-sh ltconfig ltmain.sh missing \
		       stamp-h.in

bin_SCRIPTS = edvi-config

EXTRA_DIST = AUTHORS README edvi.pc.in edvi-config.in

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = edvi.pc
