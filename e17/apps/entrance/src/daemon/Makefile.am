## Process this file with automake to produce Makefile.in

INCLUDES = @x_cflags@ @ecore_cflags@ @edb_cflags@

sbin_PROGRAMS = entranced
bin_SCRIPTS = entrance_wrapper
localstate_DATA = 

entranced_SOURCES = \
	Entranced.h spawner.c md5.c md5.h auth.c util.c

entranced_LDADD = @x_libs@ -lXau @ecore_libs@ @edb_libs@

