MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir)/src/lib \
-I$(top_builddir)/src/lib \
-I$(top_srcdir)/src/engines/evas_software_ddraw \
-I$(top_builddir)/src/engines/evas_software_ddraw \
@ECORE_WIN32_CFLAGS@ \
@ECORE_CFLAGS@ \
@EVAS_CFLAGS@

if EWL_ENABLE_EVAS_SOFTWARE_DDRAW

pkgdir = $(libdir)/ewl/engines

pkg_LTLIBRARIES = evas_software_ddraw.la

evas_software_ddraw_la_SOURCES = \
Ewl_Engine_Evas_Software_DDraw.h \
ewl_engine_evas_software_ddraw.c

evas_software_ddraw_la_LIBADD = $(top_builddir)/src/lib/libewl.la @ECORE_WIN32_LIBS@ @ECORE_LIBS@ @EVAS_LIBS@
evas_software_ddraw_la_LDFLAGS = @lt_no_undefined@ @lt_enable_auto_import@ -module -version-info @INTERFACE_VERSION@
evas_software_ddraw_la_DEPENDENCIES =

endif

