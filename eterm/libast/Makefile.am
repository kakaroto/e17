# $Id$

AUTOMAKE_OPTIONS = foreign
SUBDIRS = . test

bin_SCRIPTS = libast-config
lib_LTLIBRARIES = libast.la
include_HEADERS = libast.h

libast_la_SOURCES = conf.c debug.c file.c mem.c msgs.c strings.c snprintf.c libast.h libast_internal.h

LIBS = -lm

libast_la_LDFLAGS = -version-info 1:2:0

EXTRA_DIST = README

test: all
	cd test && $(MAKE) test

.PHONY: test
