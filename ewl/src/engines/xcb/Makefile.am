MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir)/src/lib \
-I$(top_builddir)/src/lib \
-I$(top_srcdir)/src/engines/xcb \
-I$(top_builddir)/src/engines/xcb \
@ECORE_CFLAGS@ \
@ECORE_XCB_CFLAGS@

pkgdir = $(libdir)/ewl/engines

if EWL_ENABLE_XCB

pkg_LTLIBRARIES = xcb.la

xcb_la_SOURCES = \
Ewl_Engine_Xcb.h \
ewl_engine_xcb.c

xcb_la_LIBADD = $(top_builddir)/src/lib/libewl.la
xcb_la_LDFLAGS = -module -version-info @INTERFACE_VERSION@
xcb_la_DEPENDENCIES =

endif
