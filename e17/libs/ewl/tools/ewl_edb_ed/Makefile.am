AUTOMAKE_OPTIONS = 1.4 foreign
MAINTAINERCLEANFILES = Makefile.in

CFLAGS			=	-g -W -Wall -Wno-implicit
INCLUDES		=	-I$(top_srcdir)/src -I$(top_builddir)/src @edje_cflags@ @ecore_cflags@ @evas_cflags@ @etox_cflags@ -I/usr/X11R6/include

bin_PROGRAMS			=	ewl_edb_ed

ewl_edb_ed_DEPENDENCIES		=	$(top_builddir)/src/libewl.la
ewl_edb_ed_SOURCES		=	ewl_edb_ed.c
ewl_edb_ed_LDADD			= $(top_builddir)/src/libewl.la @edje_libs@ @ecore_libs@ @evas_libs@ @emotion_libs@
