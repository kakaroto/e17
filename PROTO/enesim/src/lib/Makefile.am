SUBDIRS = drawer rasterizer renderer util primitives format

MAINTAINERCLEANFILES = \
Makefile.in

AM_CPPFLAGS = \
-I. \
-I$(top_srcdir)/src/include \
@ENESIM_CFLAGS@

lib_LTLIBRARIES = libenesim.la

libenesim_la_SOURCES = \
enesim_main.c \
enesim_format.c \
enesim_converter.c \
enesim_drawer.c \
enesim_matrix.c \
enesim_rasterizer.c \
enesim_renderer.c \
enesim_surface.c

libenesim_la_LIBADD = \
format/libenesim_format.la \
util/libenesim_util.la \
rasterizer/libenesim_rasterizer.la \
renderer/libenesim_renderer.la \
drawer/libenesim_drawer.la \
primitives/libenesim_primitives.la \
@ENESIM_LIBS@

libenesim_la_DEPENDENCIES = \
format/libenesim_format.la \
util/libenesim_util.la \
rasterizer/libenesim_rasterizer.la \
drawer/libenesim_drawer.la \
format/libenesim_format.la \
renderer/libenesim_renderer.la \
primitives/libenesim_primitives.la

libenesim_la_LDFLAGS = @create_shared_lib@ -version-info @version_info@
