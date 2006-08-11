## Process this file with automake to produce Makefile.in

INCLUDES = @X_CFLAGS@ @ECORE_CFLAGS@

entrancelibdir = $(libdir)/$(PACKAGE)

bin_PROGRAMS=entrance_edit2
sbin_PROGRAMS = entranced
entrancelib_SCRIPTS = entrance_wrapper

entranced_SOURCES = \
	Entranced.h \
	auth.c auth.h \
	ipc.c ipc.h \
	md5.c md5.h \
	util.c util.h \
	spawner.c \
	Entranced_Display.h entranced_display.c

entranced_LDADD = @X_LIBS@ -lXau @ECORE_LIBS@

entrance_edit2_SOURCES = Entranced.h entrance_edit_main.c Entrance_Edit.h
entrance_edit2_LDADD = @ECORE_LIBS@ -lentrance_edit
