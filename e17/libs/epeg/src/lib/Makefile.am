## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS     = 1.4 foreign

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = Makefile.in

LDFLAGS		     = -L/usr/local/lib
INCLUDES	     = -I/usr/local/include \
		       -I$(includedir) \
		       @my_includes@

lib_LTLIBRARIES      = libepeg.la
include_HEADERS      = Epeg.h
libepeg_la_SOURCES  = \
epeg_main.c \
epeg_memfile.c \
epeg_private.h

libepeg_la_LIBADD       = $(LDFLAGS) @my_libs@
libepeg_la_DEPENDENCIES = $(top_builddir)/config.h
libepeg_la_LDFLAGS      = -version-info 9:0:9
