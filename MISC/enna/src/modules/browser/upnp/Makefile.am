MAINTAINERCLEANFILES = Makefile.in

MODULE = upnp

# data files for the module
filesdir = $(libdir)/enna/modules/

# the module .so file
INCLUDES = \
-I. \
-I$(top_srcdir) \
-I$(top_srcdir)/src/modules/$(MODULE) \
-I$(top_srcdir)/src/bin \
-I$(top_srcdir)/src/modules \
@ENNA_CFLAGS@ @GUPNP_CFLAGS@

if BUILD_BROWSER_UPNP

pkgdir = $(libdir)/enna/modules

pkg_LTLIBRARIES = browser_upnp.la

browser_upnp_la_SOURCES= \
enna_module_upnp.c

browser_upnp_la_LIBADD = @ENNA_LIBS@ @GUPNP_LIBS@
browser_upnp_la_LDFLAGS = -module -avoid-version
browser_upnp_la_DEPENDENCIES = $(top_builddir)/config.h

uninstall:
	rm -rf $(DESTDIR)$(libdir)/enna/modules/browser_$(MODULE).*

endif
