## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS     = 1.4 foreign

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = Makefile.in

LDFLAGS		     = -L/usr/local/lib @imlib_libs@ @epeg_libs@
INCLUDES	     = -I/usr/local/include \
		       -I$(includedir) @imlib_cflags@ @epeg_cflags@

lib_LTLIBRARIES      = libepsilon.la
include_HEADERS      = Epsilon.h
libepsilon_la_SOURCES  = \
Epsilon.c \
md5.c

libepsilon_la_LIBADD       = $(LDFLAGS)
libepsilon_la_DEPENDENCIES = ../config.h
libepsilon_la_LDFLAGS      = -version-info 0:1:0
