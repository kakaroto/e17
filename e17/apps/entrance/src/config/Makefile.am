## Process this file with automake to produce Makefile.in
MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I$(srcdir)/../lib @X_CFLAGS@ @ECORE_CFLAGS@

bin_PROGRAMS = entrance_edit

entrance_edit_SOURCES = ../daemon/Entranced.h entrance_edit_main.c ../lib/Entrance_Edit.h
entrance_edit_LDADD = @ECORE_LIBS@ -lentrance_edit
entrance_edit_CFLAGS = @ECORE_CFLAGS@
entrance_edit_LDFLAGS = -L../lib
