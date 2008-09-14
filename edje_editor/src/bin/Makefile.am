## Process this file with automake to produce Makefile.in
MAINTAINERCLEANFILES = Makefile.in

INCLUDES = \
-Wall -g \
-I$(top_srcdir)/lib \
@ECORE_CFLAGS@ \
@ETK_CFLAGS@ \
@EVAS_CFLAGS@ \
@EDJE_CFLAGS@

bin_PROGRAMS = edje_editor

edje_editor_INCLUDES = \
edje_editor_images.h \
edje_editor_tree.h \
edje_editor_group.h \
edje_editor_part.h \
edje_editor_state.h \
edje_editor_position.h \
edje_editor_text.h \
edje_editor_rect.h \
edje_editor_program.h \
edje_editor_script.h \
edje_editor_toolbar.h \
edje_editor_canvas.h \
edje_editor_consolle.h \
edje_editor_dialogs.h \
edje_editor_window.h \
edje_editor_gradient.h \
edje_editor_fill.h \
edje_editor_spectra.h \
edje_editor_spectra_widget.h \
edje_editor_tree_model_spectra.h \
edje_editor_data.h \
main.h


edje_editor_SOURCES = \
edje_editor_images.c \
edje_editor_tree.c \
edje_editor_group.c \
edje_editor_part.c \
edje_editor_state.c \
edje_editor_position.c \
edje_editor_text.c \
edje_editor_rect.c \
edje_editor_program.c \
edje_editor_script.c \
edje_editor_toolbar.c \
edje_editor_canvas.c \
edje_editor_consolle.c \
edje_editor_dialogs.c \
edje_editor_window.c \
edje_editor_gradient.c \
edje_editor_fill.c \
edje_editor_spectra.c \
edje_editor_spectra_widget.c \
edje_editor_tree_model_spectra.c \
edje_editor_data.c \
main.c \
$(edje_editor_INCLUDES)

edje_editor_LDADD = @ECORE_LIBS@ @ETK_LIBS@ @EVAS_LIBS@ @EDJE_LIBS@
