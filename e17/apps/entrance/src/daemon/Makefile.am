## Process this file with automake to produce Makefile.in

INCLUDES = @ebg_cflags@ @ecore_cflags@ @edb_cflags@

sbin_PROGRAMS = entranced
bin_SCRIPTS = entrance_wrapper

entranced_SOURCES = \
	spawner.c spawner.h    

entranced_LDADD = @ebg_libs@ @ecore_libs@ @edb_libs@ @evas_libs@

