## Process this file with automake to produce Makefile.in
MAINTAINERCLEANFILES = Makefile.in

INCLUDES = @ELEMENTARY_CFLAGS@ @ECORE_CFLAGS@

if BUILD_CONFIGGUI

bin_PROGRAMS = entrance-edit-elm

entrance_edit_elm_SOURCES = main.c

entrance_edit_elm_LDADD = @ELEMENTARY_LIBS@ @ECORE_LIBS@
	
endif

EXTRA_DIST = main.c
