MAINTAINERCLEANFILES = Makefile Makefile.in

AUTOMAKE_OPTIONS = 1.4 foreign

INCLUDES = -I$(top_srcdir)/src/lib \
		-I$(top_builddir)/src/lib \
		-I$(top_srcdir)/src/engines/evas \
		-I$(top_builddir)/src/engines/evas \
		@ECORE_CFLAGS@

pkgdir = $(libdir)/ewl/engines

if EWL_ENABLE_EVAS_SOFTWARE_X11

pkg_LTLIBRARIES = evas.la

evas_la_SOURCES = \
	Ewl_Engine_Evas.h \
	ewl_engine_evas.c

evas_la_LIBADD = $(top_builddir)/src/lib/libewl.la
evas_la_LDFLAGS = -module -avoid-version \
		-L$(top_builddir)/src/lib \
		-L$(top_builddir)/src/lib/.libs
evas_la_DEPENDENCIES =

endif
