## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS     = 1.4 foreign

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = Makefile.in

LDFLAGS		     = -L/usr/X11R6/lib -L/usr/local/lib
INCLUDES	     = -I/usr/X11R6/include -I/usr/local/include \
			$(X_CFLAGS) -I$(includedir) \
			-DLIBDIR=\"$(libdir)\" \
			-DBINDIR=\"$(bindir)\" \
			-DDATADIR=\"$(datadir)\"

lib_LTLIBRARIES      = libedb.la
include_HEADERS      = \
Edb.h

libedb_la_SOURCES   = \
e_db.c \
Edb.h

libedb_la_LIBADD       = -ldb $(LDFLAGS)
libedb_la_DEPENDENCIES = $(top_builddir)/config.h
libedb_la_LDFLAGS      = -version-info 0:1:0
