MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir)/src/lib \
-I$(top_builddir)/src/lib \
-I$(top_srcdir)/src/engines/evas \
-I$(top_builddir)/src/engines/evas \
@EVAS_CFLAGS@ \
@ECORE_CFLAGS@ \
@EDJE_CFLAGS@ \
@WIN32_CFLAGS@

pkgdir = $(libdir)/ewl/engines

if EWL_ENABLE_EVAS

pkg_LTLIBRARIES = evas.la

evas_la_SOURCES = \
Ewl_Engine_Evas.h \
ewl_engine_evas.c

evas_la_LIBADD = $(top_builddir)/src/lib/libewl.la @EDJE_LIBS@ @ECORE_LIBS@ @EVAS_LIBS@
evas_la_LDFLAGS = @lt_no_undefined@ @lt_enable_auto_import@ -module -version-info @INTERFACE_VERSION@
evas_la_DEPENDENCIES =

endif
