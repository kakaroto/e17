## Process this file with automake to produce Makefile.in

INCLUDES = -I$(top_srcdir)/src @ebits_cflags@ @ecore_cflags@ @edb_cflags@ @evas_cflags@ @ewd_cflags@ @etox_cflags@

fade_outdir			= $(pkgdatadir)/plugins/fx

fade_out_LTLIBRARIES	= fade_out.la

fade_out_la_SOURCES	= fade_out.c
fade_out_la_LDFLAGS      = -no-undefined -module -avoid-version
fade_out_la_LIBADD	= @ebits_libs@ @ecore_libs@ @edb_libs@ @evas_libs@ @ewd_libs@ @etox_libs@ -lm
