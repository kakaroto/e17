## $Id$

SUBDIRS = lib plugins include

AM_CFLAGS = -I$(top_srcdir)/src/include \
            @EMBRACE_CFLAGS@

bin_PROGRAMS = embrace

embrace_SOURCES = main.c

embrace_LDADD = $(top_builddir)/src/lib/libembrace.la \
                @EMBRACE_LIBS@ \
                @DL_LIBS@

embrace_LDFLAGS = -export-dynamic
embrace_DEPENDENCIES = $(top_builddir)/src/lib/libembrace.la
