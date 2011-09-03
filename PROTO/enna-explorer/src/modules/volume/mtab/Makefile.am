MAINTAINERCLEANFILES = Makefile.in

# data files for the module
filesdir = $(libdir)/enna-explorer/modules/

AM_CPPFLAGS = \
-I$(top_srcdir)/src/modules/mtab \
-I$(top_srcdir)/src/bin \
@ENNA_CFLAGS@

pkgdir = $(libdir)/enna-explorer/modules

pkg_LTLIBRARIES = volume_mtab.la

volume_mtab_la_SOURCES = mtab.c

volume_mtab_la_LIBADD = @ENNA_LIBS@
volume_mtab_la_LDFLAGS = -module -avoid-version
