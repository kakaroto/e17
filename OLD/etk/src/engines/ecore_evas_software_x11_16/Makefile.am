MAINTAINERCLEANFILES = Makefile Makefile.in

AUTOMAKE_OPTIONS = 1.4 foreign

INCLUDES = -I$(top_srcdir)/src/lib \
	   -I$(top_srcdir)/src/engines/ecore_evas \
	   -I$(top_srcdir)/src/engines/ecore_evas_x11 \
	   -I$(top_srcdir)/src/engines/ecore_evas_software_x11 \
	   -I$(top_srcdir)/src/engines/ecore_evas_software_x11_16 \
	   @EVAS_CFLAGS@ @ECORE_CFLAGS@ @ECORE_X_CFLAGS@ -g -Wall

pkgdir = $(libdir)/etk/engines

if BUILD_ENGINE_EE_S_X11

pkg_LTLIBRARIES = ecore_evas_software_x11_16.la

ecore_evas_software_x11_16_la_SOURCES = \
	ecore_evas_software_x11_16.c

ecore_evas_software_x11_16_la_LIBADD = $(top_builddir)/src/lib/libetk.la
ecore_evas_software_x11_16_la_LDFLAGS = -module -avoid-version \
				     -L$(top_builddir)/src/lib \
		                     -L$(top_builddir)/src/lib/.libs
ecore_evas_software_x11_16_la_DEPENDENCIES =

endif
