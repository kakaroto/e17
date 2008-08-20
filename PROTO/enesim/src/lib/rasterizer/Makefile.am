MAINTAINERCLEANFILES = \
Makefile.in

AM_CPPFLAGS = \
-I. \
-I$(top_srcdir)/src/include \
@ENESIM_CFLAGS@

noinst_LTLIBRARIES = libenesim_rasterizer.la

libenesim_rasterizer_la_SOURCES = \
cpsc.c \
kiia.c \
kiia8.c \
kiia16.c

libenesim_rasterizer_la_LIBADD = -lm
