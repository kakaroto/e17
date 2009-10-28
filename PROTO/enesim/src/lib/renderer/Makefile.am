SUBDIRS = gradient shape filter

MAINTAINERCLEANFILES = \
Makefile.in

AM_CPPFLAGS = \
-I. \
-I$(top_srcdir)/src/include \
@ENESIM_CFLAGS@

noinst_LTLIBRARIES = libenesim_renderer.la

libenesim_renderer_la_SOURCES = \
surface.c \
checker.c \
hswitch.c \
stripes.c \
shape.c \
importer.c \
perlin.c \
gradient.c \
compound.c \
transition.c \
grid.c

libenesim_renderer_la_LIBADD = \
gradient/libenesim_renderer_gradient.la \
shape/libenesim_renderer_shape.la \
filter/libenesim_renderer_filter.la

libenesim_renderer_la_DEPENDENCIES = \
gradient/libenesim_renderer_gradient.la \
shape/libenesim_renderer_shape.la \
filter/libenesim_renderer_filter.la

