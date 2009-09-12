## Process this file with automake to produce Makefile.in

SUBDIRS = src

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess \
                       config.h.in config.sub configure install-sh \
		       ltconfig ltmain.sh missing mkinstalldirs \
		       stamp-h.in epsilon_docs.tar epsilon.c \
		       epsilon.spec epsilon.pc depcomp

EXTRA_DIST = README AUTHORS COPYING epsilon.spec.in epsilon.spec gendoc \
	     Doxyfile epsilon.pc.in

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = epsilon.pc
