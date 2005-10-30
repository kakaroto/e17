AUTOMAKE_OPTIONS = 1.4 foreign
MAINTAINERCLEANFILES = Makefile.in

AM_CFLAGS		=	-W -Wall -Wno-implicit
INCLUDES		=	-I$(top_srcdir)/src @edje_cflags@ @ecore_cflags@ @evas_cflags@ -I/usr/X11R6/include

bin_PROGRAMS			=	ewl_config

ewl_config_DEPENDENCIES		=	$(top_builddir)/src/libewl.la
ewl_config_SOURCES		=	ewl_config.c
ewl_config_LDADD			= $(top_builddir)/src/libewl.la @edje_libs@ @ecore_libs@ @evas_libs@ @emotion_libs@
