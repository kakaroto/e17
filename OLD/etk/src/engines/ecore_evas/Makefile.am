MAINTAINERCLEANFILES = Makefile Makefile.in

AUTOMAKE_OPTIONS = 1.4 foreign

INCLUDES = -I$(top_srcdir)/src/lib \
	   -I$(top_srcdir)/src/engines/ecore_evas \
	   @EVAS_CFLAGS@ @ECORE_CFLAGS@ -g -Wall

pkgdir = $(libdir)/etk/engines

pkg_LTLIBRARIES = ecore_evas.la

include_HEADERS = Etk_Engine_Ecore_Evas.h
ecore_evas_la_SOURCES = \
	ecore_evas.c

ecore_evas_la_LIBADD = $(top_builddir)/src/lib/libetk.la
ecore_evas_la_LDFLAGS = -module -avoid-version \
				     -L$(top_builddir)/src/lib \
		                     -L$(top_builddir)/src/lib/.libs
ecore_evas_la_DEPENDENCIES =
