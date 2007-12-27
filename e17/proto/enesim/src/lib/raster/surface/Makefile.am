SUBDIRS = argb8888

MAINTAINERCLEANFILES = \
Makefile.in

INCLUDES = -I. \
-I$(top_srcdir)/src/lib \
-I$(top_srcdir)/src/lib/util \
@EDATA_CFLAGS@

noinst_LTLIBRARIES = libenesim_surface.la

libenesim_surface_la_SOURCES = \
surface.c \
argb8888.c \
rgb565.c

libenesim_surface_la_LIBADD = \
argb8888/libenesim_surface_argb8888.la

libenesim_surface_la_DEPENDENCIES = \
argb8888/libenesim_surface_argb8888.la

