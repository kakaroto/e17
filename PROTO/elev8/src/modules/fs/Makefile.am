AUTOMAKE_OPTIONS = 1.4 foreign

MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir) \
-I$(top_srcdir)/src/include \
-DPACKAGE_BIN_DIR=\"$(bindir)\" \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
-DPACKAGE_TMP_DIR=\"/tmp\" \
@EIO_CFLAGS@

moddir = $(libdir)/$(PACKAGE)/$(MODULE_ARCH)
mod_LTLIBRARIES = libfs.la

libfs_la_SOURCES = fs.cc
libfs_la_LIBADD = @EIO_LIBS@ @V8_LIBS@
libfs_la_LDFLAGS = -module -avoid-version -no-undefined
libfs_la_LIBTOOLFLAGS = --tag=disable-static
