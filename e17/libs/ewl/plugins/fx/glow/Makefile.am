## Process this file with automake to produce Makefile.in

INCLUDES = -I$(top_srcdir)/src @ebits_cflags@ @ecore_cflags@ @edb_cflags@ @evas_cflags@ @ewd_cflags@ @etox_cflags@

glowdir			= $(pkgdatadir)/plugins/fx

glow_LTLIBRARIES	= glow.la

glow_la_SOURCES	= glow.c
glow_la_LDFLAGS      = -no-undefined -module -avoid-version
glow_la_LIBADD	= @ebits_libs@ @ecore_libs@ @edb_libs@ @evas_libs@ @ewd_libs@ @etox_libs@ -lm
