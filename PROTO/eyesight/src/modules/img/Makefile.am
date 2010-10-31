MAINTAINERCLEANFILES = Makefile.in


if EYESIGHT_BUILD_IMG
if !EYESIGHT_STATIC_BUILD_IMG

installed_headersdir = $(includedir)/eyesight
include_HEADERS = Eyesight_Module_Img.h

pkgdir = $(libdir)/eyesight

pkg_LTLIBRARIES = img.la
img_la_SOURCES = \
eyesight_img.c \
eyesight_img.h
img_la_CPPFLAGS = \
-I$(top_srcdir) \
-I$(top_srcdir)/src/lib \
@EYESIGHT_CFLAGS@ \
@ECORE_FILE_CFLAGS@
img_la_LIBADD  = @EYESIGHT_LIBS@ @ECORE_FILE_LIBS@ $(top_builddir)/src/lib/libeyesight.la
img_la_LDFLAGS = -no-undefined @lt_enable_auto_import@ -module -avoid-version
img_la_LIBTOOLFLAGS = --tag=disable-static

endif
endif
