MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir)/src/lib \
-I$(top_builddir)/src/lib \
-I$(top_srcdir)/src/engines/evas_fb \
-I$(top_builddir)/src/engines/evas_fb \
@EVAS_CFLAGS@ \
@ECORE_CFLAGS@ \
@ECORE_FB_CFLAGS@

pkgdir = $(libdir)/ewl/engines

if EWL_ENABLE_EVAS_FB

pkg_LTLIBRARIES = evas_fb.la

evas_fb_la_SOURCES = \
Ewl_Engine_Evas_Fb.h \
ewl_engine_evas_fb.c

evas_fb_la_LIBADD = $(top_builddir)/src/lib/libewl.la  @ECORE_FB_LIBS@
evas_fb_la_LDFLAGS = -module -version-info @INTERFACE_VERSION@
evas_fb_la_DEPENDENCIES =

endif

