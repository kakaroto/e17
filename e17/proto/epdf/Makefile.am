ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 compile \
		       config.guess config.h.in config.sub configure \
		       depcomp install-sh ltconfig ltmain.sh missing \
		       stamp-h.in

bin_SCRIPTS = epdf-config

EXTRA_DIST = AUTHORS README epdf.pc.in epdf-config.in

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = epdf.pc
