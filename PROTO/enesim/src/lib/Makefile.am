SUBDIRS = cpu util drawer rasterizer converter scaler transformer raddist dispmap

MAINTAINERCLEANFILES = \
Makefile.in

AM_CPPFLAGS = \
-I. \
-I$(top_srcdir)/src/include \
@ENESIM_CFLAGS@

lib_LTLIBRARIES = libenesim.la

libenesim_la_SOURCES = \
enesim_main.c \
enesim_cpu.c \
enesim_format.c \
enesim_surface.c \
enesim_rasterizer.c \
enesim_operator.c \
enesim_converter.c \
enesim_drawer.c \
enesim_reflector.c \
enesim_repeater.c \
enesim_rotator.c \
enesim_scaler.c \
enesim_transformer.c \
enesim_raddist.c \
enesim_dispmap.c

libenesim_la_LIBADD = \
cpu/libenesim_cpu.la \
util/libenesim_util.la \
rasterizer/libenesim_rasterizer.la \
drawer/libenesim_drawer.la \
converter/libenesim_converter.la \
scaler/libenesim_scaler.la \
transformer/libenesim_transformer.la \
raddist/libenesim_raddist.la \
dispmap/libenesim_dispmap.la \
@ENESIM_LIBS@

libenesim_la_DEPENDENCIES = \
cpu/libenesim_cpu.la \
util/libenesim_util.la \
rasterizer/libenesim_rasterizer.la \
drawer/libenesim_drawer.la \
converter/libenesim_converter.la \
scaler/libenesim_scaler.la \
transformer/libenesim_transformer.la \
raddist/libenesim_raddist.la \
dispmap/libenesim_dispmap.la

libenesim_la_LDFLAGS = -no-undefined -version-info @version_info@
