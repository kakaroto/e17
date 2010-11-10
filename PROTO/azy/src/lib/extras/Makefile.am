noinst_LTLIBRARIES = \
libcJSON.la \
libb64.la

libcJSON_la_SOURCES = cJSON.c
libcJSON_la_CFLAGS = \
 -I$(top_srcdir) \
 -I$(top_srcdir)/src/include \
 -I$(top_srcdir)/src/include/extras
libcJSON_la_LIBADD = -lm

libb64_la_SOURCES = cencode.c cdecode.c
libb64_la_CFLAGS =  \
 -I$(top_srcdir) \
 -I$(top_srcdir)/src/include \
 -I$(top_srcdir)/src/include/extras
libb64_la_LIBADD = -lm

MAINTAINERCLEANFILES = Makefile.in
