## $Id$

AM_CFLAGS = -I. \
	    -I$(top_srcdir)/src/include \
            @EDB_CFLAGS@    \
            @EVAS_CFLAGS@   \
            @ECORE_CFLAGS@ \
	    @ESMART_CFLAGS@

noinst_LTLIBRARIES = libembrace.la

libembrace_la_SOURCES = embrace.c \
                        embrace_plugin.c \
                        mailbox.c

libembrace_la_DEPENDENCIES = $(top_builddir)/config.h
