ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = azy.pc

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess config.h* config.sub \
	configure depcomp install-sh ltmain.sh missing compile m4/l*

maintainer-clean-local:
	rm -rf autom4te.cache
