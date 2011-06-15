
MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir)/src/enlil \
-I$(top_builddir)/src/enlil \
-I$(top_srcdir)/src/enlil/exif \
-I$(top_builddir)/src/enlil/exif \
-I$(top_srcdir)/src/enlil/exif/exif \
-I$(top_builddir)/src/enlil/exif/exif \
@ENLIL_CFLAGS@ \
@EFL_ENLIL_BUILD@

lib_LTLIBRARIES = libenlil.la

libenlil_la_SOURCES = 	\
	enlil_main.c	\
	enlil_data.c	\
	enlil_alloc.c	\
	enlil_library.c

installed_headersdir = $(includedir)/enlil-$(VMAJ)
installed_headers_DATA = Enlil.h
libenlil_la_LIBADD = @ENLIL_LIBS@
libenlil_la_LDFLAGS = -no-undefined -version-info @version_info@


