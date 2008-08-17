## Process this file with automake to produce Makefile.in
MAINTAINERCLEANFILES = Makefile.in

INCLUDES = @X_CFLAGS@ @ECORE_CFLAGS@

sbin_PROGRAMS = entranced
bin_SCRIPTS = entrance_wrapper

entranced_SOURCES = \
	Entranced.h \
	auth.c auth.h \
	ipc.c ipc.h \
	md5.c md5.h \
	util.c util.h \
	spawner.c \
	Entranced_Display.h entranced_display.c

entranced_LDADD = @X_LIBS@ -lXau @ECORE_LIBS@
