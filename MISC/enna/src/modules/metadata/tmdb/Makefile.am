MAINTAINERCLEANFILES = Makefile.in

MODULE = tmdb

# data files for the module
filesdir = $(libdir)/enna/modules/

# the module .so file
INCLUDES = \
-I. \
-I$(top_srcdir) \
-I$(top_srcdir)/src/modules/$(MODULE) \
-I$(top_srcdir)/src/bin \
-I$(top_srcdir)/src/modules \
@ENNA_CFLAGS@ @LIBCURL_CFLAGS@ @LIBXML2_CFLAGS@

if BUILD_METADATA_TMDB
if BUILD_LIBXML2
if BUILD_LIBCURL

pkgdir = $(libdir)/enna/modules

pkg_LTLIBRARIES = metadata_tmdb.la

metadata_tmdb_la_SOURCES= \
tmdb.c

metadata_tmdb_la_LIBADD = @ENNA_LIBS@ @LIBCURL_LIBS@ @LIBXML2_LIBS@
metadata_tmdb_la_LDFLAGS = -module -avoid-version
metadata_tmdb_la_DEPENDENCIES = $(top_builddir)/config.h

uninstall:
	rm -rf $(DESTDIR)$(libdir)/enna/modules/metadata_$(MODULE).*

endif
endif
endif
