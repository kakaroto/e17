## Process this file with automake to produce Makefile.in
## Created by Anjuta

SUBDIRS = src \
	data

eflxx_examplesdocdir = ${prefix}/doc/eflxx_examples
eflxx_examplesdoc_DATA = \
	README\
	COPYING\
	AUTHORS\
	ChangeLog\
	INSTALL\
	NEWS


EXTRA_DIST = $(eflxx_examplesdoc_DATA)


# Remove doc directory on uninstall
uninstall-local:
	-rm -r $(eflxx_examplesdocdir)
