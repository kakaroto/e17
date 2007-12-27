MAINTAINERCLEANFILES = \
Makefile.in

INCLUDES = -I. \
-I$(top_srcdir)/src/lib \
-I$(top_srcdir)/src/lib/raster/scanline \
-I$(top_srcdir)/src/lib/raster/surface \
-I$(top_srcdir)/src/lib/util \
@EDATA_CFLAGS@

noinst_LTLIBRARIES = libenesim_renderer.la

libenesim_renderer_la_SOURCES = \
renderer.c \
fill_color.c
# fill_surface.c

libenesim_renderer_la_LIBADD = 

libenesim_renderer_la_DEPENDENCIES =
