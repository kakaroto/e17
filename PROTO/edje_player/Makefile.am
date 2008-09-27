## Process this file with automake to produce Makefile.in
## Created by Anjuta

SUBDIRS = src

edje_playerdocdir = ${prefix}/doc/edje_player
edje_playerdoc_DATA = \
	README\
	COPYING\
	AUTHORS\
	ChangeLog\
	INSTALL\
	NEWS

INTLTOOL_FILES = \
	intltool-extract.in \
	intltool-merge.in \
	intltool-update.in \
	intltool-update \
	intltool-merge \
	intltool-extract

EXTRA_DIST = $(edje_playerdoc_DATA) \
	     $(INTLTOOL_FILES)

DISTCLEANFILES = $(INTLTOOL_FILES)

# Copy all the spec files. Of cource, only one is actually used.
dist-hook:
	for specfile in *.spec; do \
		if test -f $$specfile; then \
			cp -p $$specfile $(distdir); \
		fi \
	done

