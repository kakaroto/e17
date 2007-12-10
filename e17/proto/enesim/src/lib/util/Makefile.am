MAINTAINERCLEANFILES = \
Makefile.in

ENESIM_UTILHEADERS = \
curve.h \
extender_int.h \
extender_float.h

installed_headersdir = $(prefix)/include/enesim
installed_headers_DATA = $(ENESIM_UTILHEADERS)

INCLUDES = -I. \
-I$(top_srcdir)/src/lib/util 

noinst_LTLIBRARIES = libenesim_util.la

libenesim_util_la_SOURCES = \
curve_decasteljau.c

libenesim_util_la_LIBADD = 

libenesim_util_la_DEPENDENCIES =

libenesim_util_la_LDFLAGS      = 
