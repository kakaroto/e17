## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS     = 1.4 foreign

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = Makefile.in

LIBS_X        = @x_ldflags@ @x_libs@
LIBS_EVAS     = @evas_libs@
LIBS_EXTRA    = -L/usr/local/lib

CFLAGS_X      = @x_cflags@
CFLAGS_EVAS   = @evas_includes@
#CFLAGS_DB    = 
CFLAGS_EXTRA  = -I$(includedir) -I$(top_srcdir) -I/usr/local/include

LIBS_ALL      = $(LIBS_X) $(LIBS_EVAS) $(LIBS_EXTRA)
CFLAGS_ALL    = $(CFLAGS_X) $(CFLAGS_EVAS) $(CFLAGS_EXTRA)

INCLUDES	     = $(CFLAGS_ALL) \
		       -DLIBDIR=\"$(libdir)\" \
		       -DBINDIR=\"$(bindir)\" \
		       -DDATADIR=\"$(datadir)\"

lib_LTLIBRARIES      = libetox.la
include_HEADERS      = Etox.h

libetox_la_SOURCES   = \
Etox.h \
etox.c \
loadfile.h \
loadfile.c


libetox_la_LIBADD       = $(LIBS_ALL)
libetox_la_DEPENDENCIES = $(top_builddir)/config.h
libetox_la_LDFLAGS      = -version-info 0:1:0
