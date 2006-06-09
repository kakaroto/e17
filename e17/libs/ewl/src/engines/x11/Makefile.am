MAINTAINERCLEANFILES = Makefile Makefile.in

AUTOMAKE_OPTIONS = 1.4 foreign

INCLUDES = -I$(top_srcdir)/src/lib \
		-I$(top_builddir)/src/lib \
		-I$(top_srcdir)/src/engines/x11 \
		-I$(top_builddir)/src/engines/x11 \
		@ECORE_CFLAGS@

pkgdir = $(libdir)/ewl/engines

pkg_LTLIBRARIES = x11.la

x11_la_SOURCES = \
	Ewl_Engine_X11.h \
	ewl_engine_x11.c

x11_la_LIBADD = $(top_builddir)/src/lib/libewl.la
x11_la_LDFLAGS = -module -avoid-version \
		-L$(top_builddir)/src/lib \
		-L$(top_builddir)/src/lib/.libs
x11_la_DEPENDENCIES =

