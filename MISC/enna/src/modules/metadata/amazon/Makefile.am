MAINTAINERCLEANFILES = Makefile.in

MODULE = amazon

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

if BUILD_METADATA_AMAZON
if BUILD_LIBXML2
if BUILD_LIBCURL

pkgdir = $(libdir)/enna/modules

pkg_LTLIBRARIES = metadata_amazon.la

metadata_amazon_la_SOURCES= \
enna_module_amazon.c

metadata_amazon_la_LIBADD = @ENNA_LIBS@ @LIBCURL_LIBS@ @LIBXML2_LIBS@
metadata_amazon_la_LDFLAGS = -module -avoid-version
metadata_amazon_la_DEPENDENCIES = $(top_builddir)/config.h

uninstall:
	rm -rf $(DESTDIR)$(libdir)/enna/modules/metadata_$(MODULE).*

endif
endif
endif
