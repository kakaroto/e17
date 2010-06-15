## Process this file with automake to produce Makefile.in
## Created by Anjuta

SUBDIRS = src \
	pixmaps

e_place_testdocdir = ${prefix}/doc/e_place_test
e_place_testdoc_DATA = \
	README\
	COPYING\
	AUTHORS\
	ChangeLog\
	INSTALL\
	NEWS


EXTRA_DIST = $(e_place_testdoc_DATA)


# Remove doc directory on uninstall
uninstall-local:
	-rm -r $(e_place_testdocdir)
