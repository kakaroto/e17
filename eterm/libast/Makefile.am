# $Id$

AUTOMAKE_OPTIONS = foreign

bin_SCRIPTS = libast-config
lib_LTLIBRARIES = libast.la
include_HEADERS = libast.h

libast_la_SOURCES = debug.c file.c mem.c msgs.c strings.c snprintf.c libast.h libast_internal.h

LIBS = -lm

libast_la_LDFLAGS = -version-info 1:0:0

EXTRA_DIST = README
