AUTOMAKE_OPTIONS 	= 	1.4 foreign
MAINTAINERCLEANFILES 	= 	Makefile.in

INCLUDES		=	-I$(top_srcdir)/src/lib \
				-I$(top_builddir)/src/lib \
				-I$(top_srcdir)/src/bin \
				-I$(top_builddir)/src/bin \
				@EDJE_CFLAGS@ \
				@ECORE_CFLAGS@ \
				@EVAS_CFLAGS@ \
				@EMOTION_CFLAGS@

pkgdir                  = $(libdir)/ewl/plugins

pkg_LTLIBRARIES		= ewl_io_manager_text_plain_plugin.la

ewl_io_manager_text_plain_plugin_la_SOURCES   = ewl_io_manager_text_plain_plugin.c
ewl_io_manager_text_plain_plugin_la_LIBADD    = $(top_builddir)/src/lib/libewl.la
ewl_io_manager_text_plain_plugin_la_LDFLAGS	= -module -avoid-version \
			  -L$(top_builddir)/src/lib \
			  -L$(top_builddir)/src/lib/.libs
ewl_io_manager_text_plain_plugin_la_DEPENDENCIES = 

