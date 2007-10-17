SUBDIRS = blend

MAINTAINERCLEANFILES = \
Makefile.in

INCLUDES = -I. \
-I$(top_srcdir)/src/lib \
-I$(top_srcdir)/src/lib/raster/surface \
@EDATA_CFLAGS@

noinst_LTLIBRARIES = libenesim_surface_rop.la

libenesim_surface_rop_la_SOURCES =

libenesim_surface_rop_la_LIBADD = \
blend/libenesim_surface_rop_blend.la

libenesim_surface_rop_la_DEPENDENCIES = \
blend/libenesim_surface_rop_blend.la

