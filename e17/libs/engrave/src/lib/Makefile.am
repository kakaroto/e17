## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS     = 1.4 foreign

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = Makefile.in

LDFLAGS		     =
INCLUDES	     = \
-I$(top_srcdir) \
-I$(top_builddir) \
-I$(includedir) \
@evas_cflags@ \
@ecore_cflags@ 

CLEANFILES = \
libengrave_la-engrave.h \
libengrave_la-engrave.c \
engrave.c

lib_LTLIBRARIES      = libengrave.la 

include_HEADERS      = Engrave.h 

libengrave_la_SOURCES  = \
engrave.y \
engrave.l \
engrave_parse.c \
engrave_load.c \
engrave_out.c

libengrave_la_YFLAGS          = -d
libengrave_la_LIBADD       = $(LDFLAGS) -lm @evas_libs@ @ecore_libs@ @LEXLIB@ 
libengrave_la_CPPFLAGS     =
libengrave_la_DEPENDENCIES = $(top_builddir)/src/config.h
libengrave_la_LDFLAGS      = -version-info 1:0:1

