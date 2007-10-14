MAINTAINERCLEANFILES = \
Makefile.in

INCLUDES = -I. \
-I$(top_srcdir)/src/lib/ \
-I$(top_srcdir)/src/lib/raster \
-I$(top_srcdir)/src/lib/raster/scanline \
@EDATA_CFLAGS@

noinst_LTLIBRARIES = libenesim_rasterizer.la

libenesim_rasterizer_la_SOURCES = \
rasterizer.c \
cpsc.c 

libenesim_rasterizer_la_LIBADD = \
 -lm

libenesim_rasterizer_la_DEPENDENCIES =

libenesim_rasterizer_la_LDFLAGS      = -version-info 1:0:0
