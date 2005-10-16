## Process this file with automake to produce Makefile.in

SUBDIRS = src data po

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess \
		config.h.in config.sub configure install-sh \
		ltconfig ltmain.sh missing mkinstalldirs \
		stamp-h.in build-stamp configure-stamp depcomp \
		etk_docs.tar.gz \
		README

bin_SCRIPTS = etk-config

EXTRA_DIST = config.rpath mkinstalldirs  \
AUTHORS \
COPYING \
gendoc \
Doxyfile \
README.in \
README \
ABOUT-NLS

pkgconfigdir = $(libdir)/pkgconfig

ACLOCAL_AMFLAGS = -I m4
