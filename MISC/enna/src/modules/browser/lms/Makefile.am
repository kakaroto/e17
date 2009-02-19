MAINTAINERCLEANFILES = Makefile.in

MODULE = lms

# data files for the module
filesdir = $(libdir)/enna/modules/

# the module .so file
INCLUDES = \
-I. \
-I$(top_srcdir) \
-I$(top_srcdir)/src/modules/$(MODULE) \
-I$(top_srcdir)/src/bin \
-I$(top_srcdir)/src/modules \
@ENNA_CFLAGS@ @LMS_CFLAGS@

if BUILD_BROWSER_LMS

pkgdir = $(libdir)/enna/modules

pkg_LTLIBRARIES = browser_lms.la

browser_lms_la_SOURCES= \
enna_module_lms.c

browser_lms_la_LIBADD = @ENNA_LIBS@ @LMS_LIBS@
browser_lms_la_LDFLAGS = -module -avoid-version
browser_lms_la_DEPENDENCIES = $(top_builddir)/config.h

uninstall:
	rm -rf $(DESTDIR)$(libdir)/enna/modules/browser_$(MODULE).*

endif
