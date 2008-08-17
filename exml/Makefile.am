## Process this file with automake to produce Makefile.in

SUBDIRS = src

EXTRA_DIST = \
	README AUTHORS COPYING exml.pc.in \
	debian/changelog \
	debian/changelog.in \
	debian/control \
	debian/copyright \
	debian/libexml0-dev.install \
	debian/libexml0.install \
	debian/rules


MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess \
	README config.h.in config.sub configure install-sh \
	ltconfig ltmain.sh missing mkinstalldirs \
	stamp-h.in debian/changelog depcomp

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = exml.pc
