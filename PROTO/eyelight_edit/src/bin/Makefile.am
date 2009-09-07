
MAINTAINERCLEANFILES = Makefile.in

AM_CFLAGS = \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
-Wall -g

INCLUDES = \
@eyelight_cflags@ \
-Wall -g

bin_PROGRAMS = eyelight-edit

eyelight_edit_SOURCES = main.c slide_utils.c slides_list.c presentation.c toolbar.c slideshow.c grid_object.c slides_grid.c tabpanel.c slide_menu.c rightpanel.c
eyelight_edit_LDADD = @eyelight_libs@
