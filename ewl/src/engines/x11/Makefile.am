MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir)/src/lib \
-I$(top_builddir)/src/lib \
-I$(top_srcdir)/src/engines/x11 \
-I$(top_builddir)/src/engines/x11 \
@ECORE_X_CFLAGS@ \
@ECORE_CFLAGS@

pkgdir = $(libdir)/ewl/engines

if EWL_ENABLE_X11

pkg_LTLIBRARIES = x11.la

x11_la_SOURCES = \
Ewl_Engine_X11.h \
ewl_engine_x11.c

x11_la_LIBADD = $(top_builddir)/src/lib/libewl.la @ECORE_X_LIBS@ @ECORE_LIBS@
x11_la_LDFLAGS = -module -version-info @INTERFACE_VERSION@
x11_la_DEPENDENCIES =

endif
