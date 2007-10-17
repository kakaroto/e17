SUBDIRS = rop

MAINTAINERCLEANFILES = \
Makefile.in

INCLUDES = -I. \
-I$(top_srcdir)/src/lib \
@EDATA_CFLAGS@

noinst_LTLIBRARIES = libenesim_surface.la

libenesim_surface_la_SOURCES = \
surface.c \
argb8888.c \
rgb565.c

libenesim_surface_la_LIBADD = \
rop/libenesim_surface_rop.la

libenesim_surface_la_DEPENDENCIES = \
rop/libenesim_surface_rop.la

