MAINTAINERCLEANFILES = Makefile Makefile.in

AUTOMAKE_OPTIONS = 1.4 foreign

INCLUDES = -I$(top_srcdir)/src/lib \
		-I$(top_builddir)/src/lib \
		-I$(top_srcdir)/src/engines/evas_xrender_x11 \
		-I$(top_builddir)/src/engines/evas_xrender_x11 \
		@ECORE_CFLAGS@ @EFREET_CFLAGS@

pkgdir = $(libdir)/ewl/engines

if EWL_ENABLE_EVAS_XRENDER_X11

pkg_LTLIBRARIES = evas_xrender_x11.la

evas_xrender_x11_la_SOURCES = \
	Ewl_Engine_Evas_Xrender_X11.h \
	ewl_engine_evas_xrender_x11.c

evas_xrender_x11_la_LIBADD = $(top_builddir)/src/lib/libewl.la
evas_xrender_x11_la_LDFLAGS = -module -version-info @INTERFACE_CURRENT@:@INTERFACE_REVISION@:@INTERFACE_AGE@ \
		-L$(top_builddir)/src/lib \
		-L$(top_builddir)/src/lib/.libs
evas_xrender_x11_la_DEPENDENCIES =

endif
