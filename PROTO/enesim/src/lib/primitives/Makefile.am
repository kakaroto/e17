MAINTAINERCLEANFILES = \
Makefile.in

AM_CPPFLAGS = \
-I. \
-I$(top_srcdir)/src/include \
@ENESIM_CFLAGS@

noinst_LTLIBRARIES = libenesim_primitives.la

libenesim_primitives_la_SOURCES = \
enesim_context.c \
rect.c \
image.c

libenesim_primitives_la_LIBADD =