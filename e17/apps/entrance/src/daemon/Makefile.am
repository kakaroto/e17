## Process this file with automake to produce Makefile.in

INCLUDES = @X_CFLAGS@ @ECORE_CFLAGS@

entrancelibdir = $(libdir)/$(PACKAGE)

sbin_PROGRAMS = entranced
entrancelib_SCRIPTS = entrance_wrapper

entranced_SOURCES = \
	auth.c auth.h Entranced.h ipc.c ipc.h md5.c md5.h spawner.c util.c util.h

entranced_LDADD = @X_LIBS@ -lXau @ECORE_LIBS@
