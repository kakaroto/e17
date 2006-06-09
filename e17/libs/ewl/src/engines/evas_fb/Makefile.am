MAINTAINERCLEANFILES = Makefile Makefile.in

AUTOMAKE_OPTIONS = 1.4 foreign

INCLUDES = -I$(top_srcdir)/src/lib \
		-I$(top_builddir)/src/lib \
		-I$(top_srcdir)/src/engines/evas_fb \
		-I$(top_builddir)/src/engines/evas_fb \
		@ECORE_CFLAGS@

pkgdir = $(libdir)/ewl/engines

pkg_LTLIBRARIES = evas_fb.la

evas_fb_la_SOURCES = \
	Ewl_Engine_Evas_Fb.h \
	ewl_engine_evas_fb.c

evas_fb_la_LIBADD = $(top_builddir)/src/lib/libewl.la
evas_fb_la_LDFLAGS = -module -avoid-version \
		-L$(top_builddir)/src/lib \
		-L$(top_builddir)/src/lib/.libs
evas_fb_la_DEPENDENCIES =

