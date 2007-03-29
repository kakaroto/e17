MAINTAINERCLEANFILES = Makefile Makefile.in

AUTOMAKE_OPTIONS = 1.4 foreign

INCLUDES = -I$(top_srcdir)/src/lib \
		-I$(top_builddir)/src/lib \
		-I$(top_srcdir)/src/engines/evas_buffer \
		-I$(top_builddir)/src/engines/evas_buffer \
		@ECORE_CFLAGS@

pkgdir = $(libdir)/ewl/engines

if EWL_ENABLE_EVAS_BUFFER

pkg_LTLIBRARIES = evas_buffer.la

evas_buffer_la_SOURCES = \
	Ewl_Engine_Evas_Buffer.h \
	ewl_engine_evas_buffer.c

evas_buffer_la_LIBADD = $(top_builddir)/src/lib/libewl.la
evas_buffer_la_LDFLAGS = -module -version-info @INTERFACE_CURRENT@:@INTERFACE_REVISION@:@INTERFACE_AGE@ \
		-L$(top_builddir)/src/lib \
		-L$(top_builddir)/src/lib/.libs
evas_buffer_la_DEPENDENCIES =

endif

