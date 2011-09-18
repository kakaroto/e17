MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = -DPKGDATADIR=\"$(pkgdatadir)\" \
		-I ../			\
		@ELEMENTARY_CFLAGS@	\
		-DLOCALEDIR=\"$(localedir)\" \
		-Wall


AM_LDFLAGS =				\
		@ELEMENTARY_LIBS@

bin_PROGRAMS = e_pattern_lock

e_pattern_lock_SOURCES =	\
	e_pattern_lock.c

EXTRA_DIST=			\
	e_pattern_lock.h	\
	gettext.h
