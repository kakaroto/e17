## Process this file with automake to produce Makefile.in
MAINTAINERCLEANFILES = Makefile.in

INCLUDES = @X_CFLAGS@ @ECORE_CFLAGS@

entrancelibdir = $(libdir)/$(PACKAGE)

lib_LTLIBRARIES = libentrance_edit.la

libentrance_edit_la_SOURCES = ../daemon/Entranced.h entrance_edit.c Entrance_Edit.h
libentrance_edit_la_HEADERS = Entrance_Edit.h
libentrance_edit_ladir = $(prefix)/include
libentrance_edit_la_LIBADD = @ECORE_LIBS@
libentrance_edit_la_CFLAGS = @ECORE_CFLAGS@

bin_SCRIPTS = entrance_edit-config
