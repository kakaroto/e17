MAINTAINERCLEANFILES = Makefile.in

MODULE = shoutcast

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

if BUILD_BROWSER_SHOUTCAST

pkgdir = $(libdir)/enna/modules

pkg_LTLIBRARIES = browser_shoutcast.la

browser_shoutcast_la_SOURCES= \
enna_module_shoutcast.c

browser_shoutcast_la_LIBADD = @ENNA_LIBS@ @LIBCURL_LIBS@ @LIBXML2_LIBS@
browser_shoutcast_la_LDFLAGS = -module -avoid-version
browser_shoutcast_la_DEPENDENCIES = $(top_builddir)/config.h

uninstall:
	rm -rf $(DESTDIR)$(libdir)/enna/modules/browser_$(MODULE).*

endif
