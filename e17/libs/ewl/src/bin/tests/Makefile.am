AUTOMAKE_OPTIONS 	= 	1.4 foreign
MAINTAINERCLEANFILES 	= 	Makefile.in

AM_CFLAGS		=	-W -Wall -Wno-implicit
INCLUDES		=	-I$(top_srcdir)/src/lib \
				-I$(top_builddir)/src/lib \
				-I$(top_srcdir)/src/bin \
				-I$(top_builddir)/src/bin \
				@EDJE_CFLAGS@ \
				@ECORE_CFLAGS@ \
				@EVAS_CFLAGS@ \
				@EMOTION_CFLAGS@

pkgdir                  = $(libdir)/ewl/tests

pkg_LTLIBRARIES		= ewl_border.la ewl_box.la ewl_button.la \
			  ewl_colordialog.la ewl_datepicker.la

ewl_border_la_SOURCES   = ewl_border.c
ewl_border_la_LIBADD    = $(top_builddir)/src/lib/libewl.la
ewl_border_la_LDFLAGS	= -module -avoid-version \
			  -L$(top_builddir)/src/lib \
			  -L$(top_builddir)/src/lib/.libs
ewl_border_la_DEPENDENCIES = 

ewl_box_la_SOURCES   = ewl_box.c
ewl_box_la_LIBADD    = $(top_builddir)/src/lib/libewl.la
ewl_box_la_LDFLAGS	= -module -avoid-version \
			  -L$(top_builddir)/src/lib \
			  -L$(top_builddir)/src/lib/.libs
ewl_box_la_DEPENDENCIES = 

ewl_button_la_SOURCES   = ewl_button.c
ewl_button_la_LIBADD    = $(top_builddir)/src/lib/libewl.la
ewl_button_la_LDFLAGS	= -module -avoid-version \
			  -L$(top_builddir)/src/lib \
			  -L$(top_builddir)/src/lib/.libs
ewl_button_la_DEPENDENCIES = 

ewl_colordialog_la_SOURCES   = ewl_colordialog.c
ewl_colordialog_la_LIBADD    = $(top_builddir)/src/lib/libewl.la
ewl_colordialog_la_LDFLAGS	= -module -avoid-version \
			  -L$(top_builddir)/src/lib \
			  -L$(top_builddir)/src/lib/.libs
ewl_colordialog_la_DEPENDENCIES = 

ewl_datepicker_la_SOURCES   = ewl_datepicker.c
ewl_datepicker_la_LIBADD    = $(top_builddir)/src/lib/libewl.la
ewl_datepicker_la_LDFLAGS	= -module -avoid-version \
			  -L$(top_builddir)/src/lib \
			  -L$(top_builddir)/src/lib/.libs
ewl_datepicker_la_DEPENDENCIES = 

EXTRA_DIST = Ewl_Test.h
