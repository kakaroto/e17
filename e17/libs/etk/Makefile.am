## Process this file with automake to produce Makefile.in

SUBDIRS = src data po intl

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess \
		config.h.in config.sub configure install-sh \
		ltconfig ltmain.sh missing mkinstalldirs \
		stamp-h.in build-stamp configure-stamp depcomp \
		etk_docs.tar.gz \
		README \
		etk.pc

bin_SCRIPTS = etk-config

EXTRA_DIST = \
AUTHORS \
COPYING \
gendoc \
Doxyfile \
README \
ABOUT-NLS \
etk.pc.in

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = etk.pc
ACLOCAL_AMFLAGS = -I m4
