AUTOMAKE_OPTIONS     = 1.4 foreign
MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I. \
-I$(top_builddir) \
-I$(top_srcdir) \
-I$(top_srcdir)/src/lib \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\"

lib_LTLIBRARIES = libfiddle.la

include_HEADERS = Fiddle.h

libfiddle_la_SOURCES = \
fiddle_priv.h \
fiddle_main.c \
Fiddle.h

libfiddle_la_CFLAGS =
libfiddle_la_LIBADD = @my_libs@
libfiddle_la_LDFLAGS = -no-undefined -version-info @version_info@
