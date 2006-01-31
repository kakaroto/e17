## Process this file with automake to produce Makefile.in

SUBDIRS = src icons 

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess config.h.in config.sub configure install-sh \
		       ltconfig ltmain.sh missing mkinstalldirs stamp-h.in

e17genmenudocdir = ${prefix}/doc/e17genmenu
e17genmenudoc_DATA = README COPYING	AUTHORS ChangeLog INSTALL NEWS

EXTRA_DIST = $(e17genmenudoc_DATA) icons autogen.sh

# Copy all the spec files. Of cource, only one is actually used.
dist-hook:
	for specfile in *.spec; do \
		if test -f $$specfile; then \
			cp -p $$specfile $(distdir); \
		fi \
	done

