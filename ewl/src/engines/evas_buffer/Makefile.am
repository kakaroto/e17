MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir)/src/lib \
-I$(top_builddir)/src/lib \
-I$(top_srcdir)/src/engines/evas_buffer \
-I$(top_builddir)/src/engines/evas_buffer \
@EVAS_CFLAGS@ \
@ECORE_CFLAGS@

pkgdir = $(libdir)/ewl/engines

if EWL_ENABLE_EVAS_SOFTWARE_BUFFER

pkg_LTLIBRARIES = evas_buffer.la

evas_buffer_la_SOURCES = \
Ewl_Engine_Evas_Buffer.h \
ewl_engine_evas_buffer.c

evas_buffer_la_LIBADD = $(top_builddir)/src/lib/libewl.la @ECORE_LIBS@ @EVAS_LIBS@
evas_buffer_la_LDFLAGS = @lt_no_undefined@ @lt_enable_auto_import@ -module -version-info @INTERFACE_VERSION@
evas_buffer_la_DEPENDENCIES =

endif

