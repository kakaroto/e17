
MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir)/src/lib \
-I$(top_builddir)/src/lib \
-I$(top_srcdir)/src/engines/win32 \
-I$(top_builddir)/src/engines/win32 \
@ECORE_CFLAGS@

if EWL_ENABLE_WIN32

pkgdir = $(libdir)/ewl/engines

pkg_LTLIBRARIES = win32.la

win32_la_SOURCES = \
Ewl_Engine_Win32.h \
ewl_engine_win32.c

win32_la_LIBADD = $(top_builddir)/src/lib/libewl.la @ECORE_WIN32_LIBS@ @ECORE_LIBS@
win32_la_LDFLAGS = @lt_no_undefined@ @lt_enable_auto_import@ -module -version-info @INTERFACE_VERSION@

endif
