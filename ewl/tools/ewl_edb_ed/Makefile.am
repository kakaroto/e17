AUTOMAKE_OPTIONS = 1.4 foreign
MAINTAINERCLEANFILES = Makefile.in

AM_CFLAGS		=	-W -Wall -Wno-implicit
INCLUDES		=	-I$(top_srcdir)/src/lib -I$(top_builddir)/src/lib @edje_cflags@ @ecore_cflags@ @evas_cflags@ @edb_cflags@ -I/usr/X11R6/include

bin_PROGRAMS			=	ewl_edb_ed

ewl_edb_ed_DEPENDENCIES		=	$(top_builddir)/src/lib/libewl.la
ewl_edb_ed_SOURCES		=	ewl_edb_ed.c
ewl_edb_ed_LDADD			= $(top_builddir)/src/lib/libewl.la @edje_libs@ @ecore_libs@ @evas_libs@ @edb_libs@ @emotion_libs@
