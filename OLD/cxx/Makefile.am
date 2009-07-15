## Process this file with automake to produce Makefile.in
## Created by Anjuta - will *NOT* be overwritten

SUBDIRS = src examples data

libeflppdocdir = ${prefix}/doc/eflpp
libeflppdoc_DATA = \
	README\
	COPYING\
	AUTHORS\
	ChangeLog\
	INSTALL\
	NEWS\
	TODO

EXTRA_DIST = $(libeflppdoc_DATA)\
	     eflpp.pc.in\
	     eflpp-uninstalled.pc.in

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = eflpp.pc

# Copy all the spec files. Of cource, only one is actually used.
dist-hook:
	for specfile in *.spec; do \
		if test -f $$specfile; then \
			cp -p $$specfile $(distdir); \
		fi \
	done
