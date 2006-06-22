MAINTAINERCLEANFILES = Makefile Makefile.in

AUTOMAKE_OPTIONS = 1.4 foreign

INCLUDES = -I$(top_srcdir)/src/lib \
		-I$(top_builddir)/src/lib \
		-I$(top_srcdir)/src/engines/evas_software_x11 \
		-I$(top_builddir)/src/engines/evas_software_x11 \
		@ECORE_CFLAGS@

pkgdir = $(libdir)/ewl/engines

if EWL_ENABLE_EVAS_SOFTWARE_X11

pkg_LTLIBRARIES = evas_software_x11.la

evas_software_x11_la_SOURCES = \
	Ewl_Engine_Evas_Software_X11.h \
	ewl_engine_evas_software_x11.c

evas_software_x11_la_LIBADD = $(top_builddir)/src/lib/libewl.la
evas_software_x11_la_LDFLAGS = -module -avoid-version \
		-L$(top_builddir)/src/lib \
		-L$(top_builddir)/src/lib/.libs
evas_software_x11_la_DEPENDENCIES =

endif
