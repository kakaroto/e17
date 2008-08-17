MAINTAINERCLEANFILES = Makefile.in

INCLUDES = \
-I. \
@EDATA_CFLAGS@ \
@ECORE_CFLAGS@

lib_LTLIBRARIES = libecore_li.la
include_HEADERS = \
Ecore_Li.h

libecore_li_la_SOURCES = \
ecore_li.c \
device.c

libecore_li_la_LIBADD = \
@EDATA_LIBS@ \
@ECORE_LIBS@
libecore_li_la_LDFLAGS = -version-info 1:0:0
