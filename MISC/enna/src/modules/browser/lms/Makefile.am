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

if BUILD_LMS_MODULE

pkgdir = $(libdir)/enna/modules

pkg_LTLIBRARIES = enna_module_lms.la

enna_module_lms_la_SOURCES= \
enna_module_lms.c

enna_module_lms_la_LIBADD = @ENNA_LIBS@ @LMS_LIBS@
enna_module_lms_la_LDFLAGS = -module -avoid-version
enna_module_lms_la_DEPENDENCIES = $(top_builddir)/config.h

uninstall:
	rm -rf $(DESTDIR)$(libdir)/enna/module/enna_module_$(MODULE).*

endif
