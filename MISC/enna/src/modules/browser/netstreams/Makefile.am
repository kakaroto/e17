MAINTAINERCLEANFILES = Makefile.in

MODULE = netstreams

# data files for the module
filesdir = $(libdir)/enna/modules/

# the module .so file
INCLUDES = \
-I. \
-I$(top_srcdir) \
-I$(top_srcdir)/src/modules/$(MODULE) \
-I$(top_srcdir)/src/bin \
-I$(top_srcdir)/src/modules \
@ENNA_CFLAGS@

if BUILD_BROWSER_NETSTREAMS

pkgdir = $(libdir)/enna/modules

pkg_LTLIBRARIES = browser_netstreams.la

browser_netstreams_la_SOURCES= \
enna_module_netstreams.c

browser_netstreams_la_LIBADD = @ENNA_LIBS@
browser_netstreams_la_LDFLAGS = -module -avoid-version
browser_netstreams_la_DEPENDENCIES = $(top_builddir)/config.h

uninstall:
	rm -rf $(DESTDIR)$(libdir)/enna/modules/browser_$(MODULE).*

endif
