## Process this file with automake to produce Makefile.in

INCLUDES = @x_cflags@ @ecore_cflags@ @edb_cflags@

sbin_PROGRAMS = entranced
bin_SCRIPTS = entrance_wrapper

entranced_SOURCES = \
	auth.c auth.h Entranced.h ipc.c ipc.h md5.c md5.h spawner.c util.c util.h

entranced_LDADD = @x_libs@ -lXau @ecore_libs@ @edb_libs@
