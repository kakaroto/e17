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

if BUILD_EMOTION_BACKEND

pkgdir =$(libdir)/enna/modules

pkg_LTLIBRARIES = enna_module_emotion.la

enna_module_emotion_la_SOURCES = \
enna_module_emotion.c

enna_module_emotion_la_LIBADD = @ENNA_LIBS@ @EMOTION_LIBS@
enna_module_emotion_la_LDFLAGS = -module -avoid-version
enna_module_emotion_la_DEPENDENCIES = $(top_builddir)/config.h

uninstall:
	rm -rf $(DESTDIR)$(libdir)/enna/module/enna_module_$(MODULE).*

endif

