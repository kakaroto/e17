## Process this file with automake to produce Makefile.in

INCLUDES = -I$(top_srcdir)/src @ebits_cflags@ @ecore_cflags@ @edb_cflags@ @evas_cflags@ @ewd_cflags@ @etox_cflags@

fade_indir			= $(DESTDIR)$(pkgdatadir)/plugins/fx

fade_in_LTLIBRARIES	= fade_in.la

fade_in_la_SOURCES	= fade_in.c
fade_in_la_LDFLAGS      = -no-undefined -module -avoid-version
fade_in_la_LIBADD	= @ebits_libs@ @ecore_libs@ @edb_libs@ @evas_libs@ @ewd_libs@ @etox_libs@ -lm
