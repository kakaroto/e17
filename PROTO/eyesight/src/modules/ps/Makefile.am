MAINTAINERCLEANFILES = Makefile.in


if EYESIGHT_BUILD_POSTSCRIPT
if !EYESIGHT_STATIC_BUILD_POSTSCRIPT

includesdir = $(includedir)/eyesight
includes_HEADERS = Eyesight_Module_Ps.h

pkgdir = $(libdir)/eyesight

pkg_LTLIBRARIES = ps.la
ps_la_SOURCES = \
eyesight_ps.c \
eyesight_ps.h
ps_la_CPPFLAGS = \
-I$(top_srcdir) \
-I$(top_srcdir)/src/lib \
@EYESIGHT_CFLAGS@ \
@POSTSCRIPT_CFLAGS@
ps_la_LIBADD = @EYESIGHT_LIBS@ @POSTSCRIPT_LIBS@ $(top_builddir)/src/lib/libeyesight.la -lm
ps_la_LDFLAGS = -no-undefined @lt_enable_auto_import@ -module -avoid-version
ps_la_LIBTOOLFLAGS = --tag=disable-static
endif
endif
