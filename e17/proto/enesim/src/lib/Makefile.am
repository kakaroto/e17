SUBDIRS = raster vector

MAINTAINERCLEANFILES = \
Makefile.in

INCLUDES = -I. \
-I$(top_srcdir)/src/lib 

lib_LTLIBRARIES = libenesim.la
include_HEADERS = Enesim.h

libenesim_la_SOURCES =

libenesim_la_LIBADD = \
raster/libenesim_raster.la \
vector/libenesim_vector.la \
@EDATA_LIBS@ \
 -lm

libenesim_la_DEPENDENCIES = \
raster/libenesim_raster.la \
vector/libenesim_vector.la 

libenesim_la_LDFLAGS      = -version-info 1:0:0
