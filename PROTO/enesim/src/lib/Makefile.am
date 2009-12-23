SUBDIRS = compositor converter rasterizer renderer util

MAINTAINERCLEANFILES = \
Makefile.in

AM_CPPFLAGS = \
-I. \
-I$(top_srcdir)/src/include \
@ENESIM_CFLAGS@

lib_LTLIBRARIES = libenesim.la

libenesim_la_SOURCES = \
enesim_main.c \
enesim_surface.c \
enesim_rasterizer.c \
enesim_renderer.c \
enesim_converter.c \
enesim_compositor.c

libenesim_la_LIBADD = \
util/libenesim_util.la \
rasterizer/libenesim_rasterizer.la \
renderer/libenesim_renderer.la \
compositor/libenesim_compositor.la \
converter/libenesim_converter.la \
@ENESIM_LIBS@

libenesim_la_DEPENDENCIES = \
util/libenesim_util.la \
rasterizer/libenesim_rasterizer.la \
renderer/libenesim_renderer.la \
compositor/libenesim_compositor.la \
converter/libenesim_converter.la

libenesim_la_LDFLAGS = -no-undefined -version-info @version_info@
