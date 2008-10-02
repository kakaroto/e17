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

if BUILD_AMAZON_MODULE
if BUILD_LIBXML2
if BUILD_LIBCURL

pkgdir = $(libdir)/enna/modules

pkg_LTLIBRARIES = enna_module_amazon.la

enna_module_amazon_la_SOURCES= \
enna_module_amazon.c

enna_module_amazon_la_LIBADD = @ENNA_LIBS@ @LIBCURL_LIBS@ @LIBXML2_LIBS@
enna_module_amazon_la_LDFLAGS = -module -avoid-version
enna_module_amazon_la_DEPENDENCIES = $(top_builddir)/config.h

uninstall:
	rm -rf $(DESTDIR)$(libdir)/enna/modules/enna_module_$(MODULE).*

endif
endif
endif
