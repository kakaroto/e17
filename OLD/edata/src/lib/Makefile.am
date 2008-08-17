MAINTAINERCLEANFILES = Makefile.in

INCLUDES = \
-I.

EDATAHEADERS = \
Edata.h \
core.h \
hash.h \
lalloc.h \
list.h \
sheap.h \
tree.h

installed_headersdir = $(prefix)/include/edata
installed_headers_DATA = $(EDATAHEADERS)

lib_LTLIBRARIES = libedata.la

libedata_la_SOURCES = \
array.c \
hash.c \
list.c \
sheap.c \
tree.c \
value.c \
private.h

libedata_la_LIBADD = -lm @dlopen_libs@ @create_shared_lib@
libedata_la_LDFLAGS = -version-info 1:0:0
