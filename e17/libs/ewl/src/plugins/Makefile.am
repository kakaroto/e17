MAINTAINERCLEANFILES 	= 	Makefile.in

AM_CPPFLAGS		=	-I$(top_srcdir)/src/lib \
				-I$(top_builddir)/src/lib \
				-I$(top_srcdir)/src/bin \
				-I$(top_builddir)/src/bin \
				@EDJE_CFLAGS@ \
				@ECORE_CFLAGS@ \
				@EVAS_CFLAGS@ \
				@EMOTION_CFLAGS@

pkgdir                  = $(libdir)/ewl/plugins

pkg_LTLIBRARIES		= \
ewl_io_manager_text_plugin.la \
ewl_io_manager_text_c_plugin.la \
ewl_io_manager_image_plugin.la

ewl_io_manager_text_plugin_la_SOURCES      = ewl_io_manager_text_plugin.c
ewl_io_manager_text_plugin_la_LIBADD       = $(top_builddir)/src/lib/libewl.la @ECORE_LIBS@
ewl_io_manager_text_plugin_la_LDFLAGS      = -module @lt_no_undefined@ -avoid-version
ewl_io_manager_text_plugin_la_DEPENDENCIES =

ewl_io_manager_text_c_plugin_la_SOURCES      = ewl_io_manager_text_c_plugin.c
ewl_io_manager_text_c_plugin_la_LIBADD       = $(top_builddir)/src/lib/libewl.la @ECORE_LIBS@
ewl_io_manager_text_c_plugin_la_LDFLAGS      = -module @lt_no_undefined@ -avoid-version
ewl_io_manager_text_c_plugin_la_DEPENDENCIES =

ewl_io_manager_image_plugin_la_SOURCES      = ewl_io_manager_image_plugin.c
ewl_io_manager_image_plugin_la_LIBADD       = $(top_builddir)/src/lib/libewl.la @ECORE_LIBS@ @EVAS_LIBS@
ewl_io_manager_image_plugin_la_LDFLAGS      = -module @lt_no_undefined@ -avoid-version
ewl_io_manager_image_plugin_la_DEPENDENCIES =
