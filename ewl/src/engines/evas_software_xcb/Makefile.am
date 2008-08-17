MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir)/src/lib \
-I$(top_builddir)/src/lib \
-I$(top_srcdir)/src/engines/evas_software_xcb \
-I$(top_builddir)/src/engines/evas_software_xcb \
@ECORE_CFLAGS@ \
@EVAS_SOFTWARE_XCB_CFLAGS@

pkgdir = $(libdir)/ewl/engines

if EWL_ENABLE_EVAS_SOFTWARE_XCB

pkg_LTLIBRARIES = evas_software_xcb.la

evas_software_xcb_la_SOURCES = \
Ewl_Engine_Evas_Software_Xcb.h \
ewl_engine_evas_software_xcb.c

evas_software_xcb_la_LIBADD = $(top_builddir)/src/lib/libewl.la
evas_software_xcb_la_LDFLAGS = -module -version-info @INTERFACE_VERSION@
evas_software_xcb_la_DEPENDENCIES =

endif
