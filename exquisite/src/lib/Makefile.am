MAINTAINERCLEANFILES = Makefile.in

lib_LTLIBRARIES = libexquisite.la

includes_HEADERS = Exquisite.h
includesdir = $(includedir)/exquisite-@VMAJ@

libexquisite_la_SOURCES = exquisite.c

libexquisite_la_CFLAGS = \
-I. \
-I$(top_srcdir)/src/lib \
-I$(top_builddir)/src/lib \
-DPACKAGE_BIN_DIR=\"$(bindir)\" \
-DPACKAGE_LIB_DIR=\"$(libdir)\" \
-DPACKAGE_DATA_DIR=\"$(pkgdatadir)\" \
@EFL_EXQUISITE_BUILD@ \
@EXQUISITE_CFLAGS@

libexquisite_la_LDFLAGS = -no-undefined @lt_enable_auto_import@ -version-info @version_info@ @release_info@
libexquisite_la_LIBADD = @EXQUISITE_LIBS@
