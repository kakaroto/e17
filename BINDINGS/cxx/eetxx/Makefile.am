## Process this file with automake to produce Makefile.in
## Created by Anjuta

SUBDIRS = src \
	include

eetxxdocdir = ${prefix}/doc/eetxx
eetxxdoc_DATA = \
	README\
	COPYING\
	AUTHORS\
	ChangeLog\
	INSTALL\
	NEWS


EXTRA_DIST = $(eetxxdoc_DATA)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = eetxx.pc

# Remove doc directory on uninstall
uninstall-local:
	-rm -r $(eetxxdocdir)
