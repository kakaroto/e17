SUBDIRS = rasterizer renderer scanline surface

MAINTAINERCLEANFILES = \
Makefile.in

INCLUDES = -I. \
-I$(top_srcdir)/src/lib \
@EDATA_CFLAGS@

noinst_LTLIBRARIES = libenesim_raster.la

libenesim_raster_la_SOURCES =

libenesim_raster_la_LIBADD = \
rasterizer/libenesim_rasterizer.la \
renderer/libenesim_renderer.la \
scanline/libenesim_scanline.la \
surface/libenesim_surface.la

libenesim_raster_la_DEPENDENCIES = \
rasterizer/libenesim_rasterizer.la \
renderer/libenesim_renderer.la \
scanline/libenesim_scanline.la \
surface/libenesim_surface.la

