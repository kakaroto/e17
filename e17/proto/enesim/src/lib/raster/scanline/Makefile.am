MAINTAINERCLEANFILES = \
Makefile.in

INCLUDES = -I. \
-I$(top_srcdir)/src/lib \
-I$(top_srcdir)/src/lib/raster \
@EDATA_CFLAGS@

noinst_LTLIBRARIES = libenesim_scanline.la

libenesim_scanline_la_SOURCES = \
scanline.c \
alias.c

libenesim_scanline_la_LIBADD = \
 -lm

libenesim_scanline_la_DEPENDENCIES =

libenesim_scanline_la_LDFLAGS      = -version-info 1:0:0
