AUTOMAKE_OPTIONS = 1.4 foreign
MAINTAINERCLEANFILES = Makefile.in

CFLAGS			=	-g -W -Wall -Wno-implicit
INCLUDES		=	-I$(top_srcdir)/src @ebits_cflags@ @ecore_cflags@ @evas_cflags@ @ewd_cflags@

bin_PROGRAMS			=	ewl_config

ewl_config_DEPENDENCIES		=	$(top_builddir)/src/libewl.la
ewl_config_SOURCES		=	ewl_config.c
ewl_config_LDADD			=	$(top_builddir)/src/libewl.la @ebits_libs@ @ecore_libs@ @evas_libs@ @ewd_libs@
