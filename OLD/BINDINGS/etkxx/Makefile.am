## Process this file with automake to produce Makefile.in
## Created by Anjuta

SUBDIRS = src \
	include

etkxxdocdir = ${prefix}/doc/etkxx
etkxxdoc_DATA = \
	README\
	COPYING\
	AUTHORS\
	ChangeLog\
	INSTALL\
	NEWS

EXTRA_DIST = $(etkxxdoc_DATA)

# Copy all the spec files. Of cource, only one is actually used.
dist-hook:
	for specfile in *.spec; do \
		if test -f $$specfile; then \
			cp -p $$specfile $(distdir); \
		fi \
	done

