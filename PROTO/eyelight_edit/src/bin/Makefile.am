
MAINTAINERCLEANFILES = Makefile.in

AM_CFLAGS = \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
-Wall -g

INCLUDES = \
@eyelight_cflags@ \
-Wall -g

bin_PROGRAMS = eyelight-edit

eyelight_edit_SOURCES = main.c slides_list.c presentation.c toolbar.c slideshow.c
eyelight_edit_LDADD = @eyelight_libs@
