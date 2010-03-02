## Process this file with automake to produce Makefile.in
## Created by Anjuta

SUBDIRS = src 

esmartxxdocdir = ${prefix}/doc/esmartxx
esmartxxdoc_DATA = \
	README\
	COPYING\
	AUTHORS\
	ChangeLog\
	INSTALL\
	NEWS


EXTRA_DIST = $(esmartxxdoc_DATA)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = \
	esmartxx_cairo.pc\
	esmartxx_container.pc\
	esmartxx_group.pc\
	esmartxx_textentry.pc

# Remove doc directory on uninstall
uninstall-local:
	-rm -r $(esmartxxdocdir)
