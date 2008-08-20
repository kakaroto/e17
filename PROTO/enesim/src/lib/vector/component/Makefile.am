MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I. \
-I$(top_srcdir)/src/lib \
-I$(top_srcdir)/src/lib/vector \
-I$(top_srcdir)/src/lib/util \
@EINA_CFLAGS@

noinst_LTLIBRARIES = libenesim_component.la

libenesim_component_la_SOURCES = \
component.c \
path.c \
reader.c \
source_csv.c \
transform.c
