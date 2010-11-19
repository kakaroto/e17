MAINTAINERCLEANFILES = Makefile.in


if EYESIGHT_BUILD_TXT
if !EYESIGHT_STATIC_BUILD_TXT

includesdir = $(includedir)/eyesight
includes_HEADERS = Eyesight_Module_Txt.h

pkgdir = $(libdir)/eyesight

pkg_LTLIBRARIES = txt.la
txt_la_SOURCES = \
eyesight_txt.c \
eyesight_txt.h
txt_la_CPPFLAGS = \
-I$(top_srcdir) \
-I$(top_srcdir)/src/lib \
@EFL_EYESIGHT_BUILD@ \
@EYESIGHT_CFLAGS@
txt_la_LIBADD  = @EYESIGHT_LIBS@ $(top_builddir)/src/lib/libeyesight.la
txt_la_LDFLAGS = -no-undefined @lt_enable_auto_import@ -module -avoid-version
txt_la_LIBTOOLFLAGS = --tag=disable-static

install-data-hook:
	rm -f $(pkgdir)/$(pkg_LTLIBRARIES)

endif
endif
