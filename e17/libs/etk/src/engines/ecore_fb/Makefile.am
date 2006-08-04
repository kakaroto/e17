MAINTAINERCLEANFILES = Makefile Makefile.in

AUTOMAKE_OPTIONS = 1.4 foreign

INCLUDES = -I$(top_srcdir)/src/lib \
	   -I$(top_srcdir)/src/engines/ecore_evas \
	   @EVAS_CFLAGS@ @ECORE_CFLAGS@ -g

pkgdir = $(libdir)/etk/engines

if BUILD_ENGINE_E_FB

pkg_LTLIBRARIES = ecore_fb.la

ecore_fb_la_SOURCES = \
	ecore_fb.c

ecore_fb_la_LIBADD = $(top_builddir)/src/lib/libetk.la
ecore_fb_la_LDFLAGS = -module -avoid-version \
			    -L$(top_builddir)/src/lib \
		            -L$(top_builddir)/src/lib/.libs
ecore_fb_la_DEPENDENCIES =

endif
