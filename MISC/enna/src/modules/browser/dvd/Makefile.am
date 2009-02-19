MAINTAINERCLEANFILES = Makefile.in

MODULE = dvd

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

if BUILD_BROWSER_DVD

pkgdir = $(libdir)/enna/modules

pkg_LTLIBRARIES = browser_dvd.la

browser_dvd_la_SOURCES= \
enna_module_dvd.c

browser_dvd_la_LIBADD = @ENNA_LIBS@
browser_dvd_la_LDFLAGS = -module -avoid-version
browser_dvd_la_DEPENDENCIES = $(top_builddir)/config.h

uninstall:
	rm -rf $(DESTDIR)$(libdir)/enna/modules/browser_$(MODULE).*

endif
