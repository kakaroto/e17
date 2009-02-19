MAINTAINERCLEANFILES = Makefile.in

MODULE = localfiles

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

if BUILD_BROWSER_LOCALFILES

pkgdir = $(libdir)/enna/modules

pkg_LTLIBRARIES = browser_localfiles.la

browser_localfiles_la_SOURCES= \
enna_module_localfiles.c

browser_localfiles_la_LIBADD = @ENNA_LIBS@
browser_localfiles_la_LDFLAGS = -module -avoid-version
browser_localfiles_la_DEPENDENCIES = $(top_builddir)/config.h

uninstall:
	rm -rf $(DESTDIR)$(libdir)/enna/modules/browser_$(MODULE).*

endif
