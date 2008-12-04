SUBDIRS = drawer rasterizer renderer util transformer

MAINTAINERCLEANFILES = \
Makefile.in

AM_CPPFLAGS = \
-I. \
-I$(top_srcdir)/src/include \
-I$(top_srcdir)/src/lib/surface \
@ENESIM_CFLAGS@

lib_LTLIBRARIES = libenesim.la

libenesim_la_SOURCES = \
enesim_main.c \
enesim_converter.c \
enesim_drawer.c \
enesim_matrix.c \
enesim_rasterizer.c \
enesim_renderer.c \
enesim_surface.c \
enesim_transformer.c

libenesim_la_LIBADD = \
util/libenesim_util.la \
rasterizer/libenesim_rasterizer.la \
renderer/libenesim_renderer.la \
drawer/libenesim_drawer.la \
transformer/libenesim_transformer.la \
@ENESIM_LIBS@

libenesim_la_DEPENDENCIES = \
util/libenesim_util.la \
rasterizer/libenesim_rasterizer.la \
drawer/libenesim_drawer.la \
renderer/libenesim_renderer.la \
transformer/libenesim_transformer.la

libenesim_la_LDFLAGS = @create_shared_lib@ -version-info @version_info@
