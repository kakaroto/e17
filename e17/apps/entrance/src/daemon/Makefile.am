## Process this file with automake to produce Makefile.in

INCLUDES = @ecore_cflags@ @edb_cflags@

sbin_PROGRAMS = entranced
bin_SCRIPTS = entrance_wrapper

entranced_SOURCES = \
	spawner.c spawner.h md5.c md5.h auth.c

entranced_LDADD = @ecore_libs@ @edb_libs@

