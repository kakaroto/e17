## Process this file with automake to produce Makefile.in

ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src 

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess \
		config.h.in config.sub configure install-sh \
		ltconfig ltmain.sh missing mkinstalldirs \
		stamp-h.in build-stamp configure-stamp depcomp \
		eon_docs.tar.gz \
		eon.pc \
		debian/changelog

bin_SCRIPTS = 

EXTRA_DIST = \
AUTHORS \
COPYING \
gendoc \
Doxyfile \
README \
ABOUT-NLS \
eon.pc.in 

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = eon.pc
