SUBDIRS = blend

MAINTAINERCLEANFILES = \
Makefile.in

INCLUDES = -I. \
-I$(top_srcdir)/src/lib \
-I$(top_srcdir)/src/lib/raster/surface

noinst_LTLIBRARIES = libenesim_surface_rop.la

libenesim_surface_rop_la_SOURCES = \
argb8888.c \
rgb565.c

libenesim_surface_rop_la_LIBADD = \
blend/libenesim_surface_rop_blend.la

libenesim_surface_rop_la_DEPENDENCIES = \
blend/libenesim_surface_rop_blend.la

