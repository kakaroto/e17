## Process this file with automake to produce Makefile.in

SUBDIRS = src data doc

MAINTAINERCLEANFILES = aclocal.m4 config.guess config.h.in \
		       config.sub configure depcomp install-sh \
		       ltmain.sh Makefile.in missing imlib2_docs.tar.gz

bin_SCRIPTS = imlib2-config

EXTRA_DIST = AUTHORS COPYING COPYING-PLAIN gendoc \
             Doxyfile \
	     README.in README \
	     imlib2.pc.in \
	     imlib2.spec.in imlib2.spec \
	     debian/changelog \
	     debian/changelog.in \
	     debian/control \
	     debian/copyright \
	     debian/imlib2-config.1 \
	     debian/imlib2-demo.install \
	     debian/imlib2-test.install \
	     debian/libimlib2-dev.doc-base \
	     debian/libimlib2-dev.install \
	     debian/libimlib2.install \
	     debian/rules

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = imlib2.pc
