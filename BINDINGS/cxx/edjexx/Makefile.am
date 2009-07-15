## Process this file with automake to produce Makefile.in
## Created by Anjuta

SUBDIRS = src \
	include

edjexxdocdir = ${prefix}/doc/edjexx
edjexxdoc_DATA = \
	README\
	COPYING\
	AUTHORS\
	ChangeLog\
	INSTALL\
	NEWS


EXTRA_DIST = $(edjexxdoc_DATA)


# Remove doc directory on uninstall
uninstall-local:
	-rm -r $(edjexxdocdir)
