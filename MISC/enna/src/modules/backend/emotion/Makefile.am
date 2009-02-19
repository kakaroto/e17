MAINTAINERCLEANFILES = Makefile.in

MODULE = emotion

# data files for the module
filesdir = $(libdir)/enna/modules/

# the module .so file
INCLUDES = \
-I. \
-I$(top_srcdir)/src/bin \
@ENNA_CFLAGS@ \
@EMOTION_CFLAGS@

if BUILD_BACKEND_EMOTION

pkgdir =$(libdir)/enna/modules

pkg_LTLIBRARIES = backend_emotion.la

backend_emotion_la_SOURCES = \
enna_module_emotion.c

backend_emotion_la_LIBADD = @ENNA_LIBS@ @EMOTION_LIBS@
backend_emotion_la_LDFLAGS = -module -avoid-version
backend_emotion_la_DEPENDENCIES = $(top_builddir)/config.h

uninstall:
	rm -rf $(DESTDIR)$(libdir)/enna/modules/backend_$(MODULE).*

endif

