MAINTAINERCLEANFILES = Makefile Makefile.in

AUTOMAKE_OPTIONS = 1.4 foreign

INCLUDES = -I$(top_srcdir)/src/lib \
	   -I$(top_srcdir)/src/engines/ecore_evas \
	   @EVAS_CFLAGS@ @ECORE_CFLAGS@ @ECORE_FB_CFLAGS@ -g -Wall

pkgdir = $(libdir)/etk/engines

if BUILD_ENGINE_E_FB

pkg_LTLIBRARIES = ecore_fb.la

include_HEADERS = Etk_Engine_Ecore_Fb.h
ecore_fb_la_SOURCES = \
	ecore_fb.c

ecore_fb_la_LIBADD = $(top_builddir)/src/lib/libetk.la
ecore_fb_la_LDFLAGS = -module -avoid-version \
			    -L$(top_builddir)/src/lib \
		            -L$(top_builddir)/src/lib/.libs
ecore_fb_la_DEPENDENCIES =

endif
