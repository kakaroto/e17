## Process this file with automake to produce Makefile.in

INCLUDES = @X_CFLAGS@ @ECORE_CFLAGS@

entrancelibdir = $(libdir)/$(PACKAGE)

bin_PROGRAMS=entrance_edit2
sbin_PROGRAMS = entranced
entrancelib_SCRIPTS = entrance_wrapper

lib_LTLIBRARIES=libentrance_edit.la

entranced_SOURCES = \
	auth.c auth.h Entranced.h ipc.c ipc.h md5.c md5.h spawner.c util.c util.h

entranced_LDADD = @X_LIBS@ -lXau @ECORE_LIBS@

entrance_edit2_SOURCES = Entranced.h entrance_edit_main.c Entrance_Edit.h
entrance_edit2_LDADD = @ECORE_LIBS@ -lentrance_edit

libentrance_edit_la_SOURCES=Entranced.h entrance_edit.c Entrance_Edit.h
