MAINTAINERCLEANFILES = \
Makefile.in

if BUILD_SURFACE_ARGB8888_UNPRE
ARGB8888_UNPRE_REQ=argb8888_unpre.c
else
ARGB8888_UNPRE_REQ=
endif

if BUILD_SURFACE_RGB565_XA5
RGB565_XA5_REQ=rgb565_xa5.c
else
RGB565_XA5_REQ=
endif

if BUILD_SURFACE_RGB565_B1A3
RGB565_B1A3_REQ=rgb565_b1a3.c
else
RGB565_B1A3_REQ=
endif

AM_CPPFLAGS = \
-I. \
-I$(top_srcdir)/src/include \
@ENESIM_CFLAGS@

noinst_LTLIBRARIES = libenesim_drawer.la

libenesim_drawer_la_SOURCES = \
generic.c \
argb8888.c
#$(ARGB8888_UNPRE_REQ) \
#$(RGB565_XA5_REQ) \
#$(RGB565_B1A3_REQ)
