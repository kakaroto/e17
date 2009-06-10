MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir)/src/lib \
-I$(top_builddir)/src/lib \
-I$(top_srcdir)/src/engines/evas_software_gdi \
-I$(top_builddir)/src/engines/evas_software_gdi \
@ECORE_WIN32_CFLAGS@ \
@ECORE_CFLAGS@ \
@EVAS_CFLAGS@

if EWL_ENABLE_EVAS_SOFTWARE_GDI

pkgdir = $(libdir)/ewl/engines

pkg_LTLIBRARIES = evas_software_gdi.la

evas_software_gdi_la_SOURCES = \
Ewl_Engine_Evas_Software_Gdi.h \
ewl_engine_evas_software_gdi.c

evas_software_gdi_la_LIBADD = $(top_builddir)/src/lib/libewl.la @ECORE_WIN32_LIBS@ @ECORE_LIBS@ @EVAS_LIBS@
evas_software_gdi_la_LDFLAGS = @lt_no_undefined@ @lt_enable_auto_import@ -module -version-info @INTERFACE_VERSION@
evas_software_gdi_la_DEPENDENCIES =

endif

