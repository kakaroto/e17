## Process this file with automake to produce Makefile.in

INCLUDES = @x_cflags@ @ecore_cflags@ @edb_cflags@

sbin_PROGRAMS = entranced
bin_SCRIPTS = entrance_wrapper
localstate_DATA = 

entranced_SOURCES = \
	auth.c auth.h Entranced.h ipc.c ipc.h md5.c md5.h spawner.c util.c

entranced_LDADD = @x_libs@ -lXau @ecore_libs@ @edb_libs@

install-data-hook:
	chmod 700 $(DESTDIR)@localstatedir@
