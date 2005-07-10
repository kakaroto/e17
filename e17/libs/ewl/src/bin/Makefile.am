AUTOMAKE_OPTIONS 		= 	1.4 foreign
MAINTAINERCLEANFILES 		= 	Makefile.in

CFLAGS				=	-g -W -Wall -Wno-implicit
INCLUDES			=	-I$(top_srcdir)/src/lib -I$(top_builddir)/src/lib @edje_cflags@ @ecore_cflags@ @evas_cflags@ @emotion_cflags@

EWLTESTPROGRAMS                 =	ewl_test ewl_embed_test ewl_simple_test
if BUILD_EMOTION_SUPPORT
EWLMEDIATESTPROGRAMS            =	ewl_media_test
else
endif

bin_PROGRAMS			=	$(EWLTESTPROGRAMS) $(EWLMEDIATESTPROGRAMS)

ewl_test_DEPENDENCIES		=	$(top_builddir)/src/lib/libewl.la
ewl_test_SOURCES		=	ewl_test.h \
					ewl_test.c \
					ewl_border_test.c \
					ewl_box_test.c \
					ewl_button_test.c \
					ewl_calendar_test.c \
					ewl_colordialog_test.c \
					ewl_colorpicker_test.c \
					ewl_combo_test.c \
					ewl_datepicker_test.c \
					ewl_dialog_test.c \
					ewl_entry_test.c \
					ewl_filedialog_test.c \
					ewl_filedialog_multi_test.c \
					ewl_fileselector_test.c \
					ewl_floater_test.c \
					ewl_image_test.c \
					ewl_imenu_test.c \
					ewl_menu_test.c \
					ewl_menu_bar_test.c \
					ewl_notebook_test.c \
					ewl_paned_test.c \
					ewl_password_test.c \
					ewl_progressbar_test.c \
					ewl_seeker_test.c \
					ewl_selectionbar_test.c \
					ewl_selectionbook_test.c \
					ewl_spinner_test.c \
					ewl_statusbar_test.c \
					ewl_table_test.c \
					ewl_text_test.c \
					ewl_theme_test.c \
					ewl_tooltip_test.c \
					ewl_tree_test.c

# ewl_test_LDFLAGS		=	
ewl_test_LDADD			=	$(top_builddir)/src/lib/libewl.la @edje_libs@ @ecore_libs@ @evas_libs@ @emotion_libs@ 

ewl_embed_test_DEPENDENCIES	=	$(top_builddir)/src/lib/libewl.la
ewl_embed_test_SOURCES		=	ewl_embed_test.c

# ewl_embed_test_LDFLAGS	=	
ewl_embed_test_LDADD		=	$(top_builddir)/src/lib/libewl.la @edje_libs@ @ecore_libs@ @evas_libs@ @emotion_libs@

ewl_simple_test_DEPENDENCIES	=	$(top_builddir)/src/lib/libewl.la
ewl_simple_test_SOURCES		=	ewl_simple_test.c

# ewl_simple_test_LDFLAGS	=	
ewl_simple_test_LDADD		=	$(top_builddir)/src/lib/libewl.la @edje_libs@ @ecore_libs@ @evas_libs@ @emotion_libs@

if BUILD_EMOTION_SUPPORT
ewl_media_test_DEPENDENCIES		=	$(top_builddir)/src/lib/libewl.la
ewl_media_test_SOURCES			=	ewl_media_test.c

# ewl_media_test_LDFLAGS		=	
ewl_media_test_LDADD			= $(top_builddir)/src/lib/libewl.la @edje_libs@ @ecore_libs@ @evas_libs@ @emotion_libs@
endif

EXTRA_DIST 			= 	$(ewl_test_SOURCES) \
					$(ewl_embed_test_SOURCES) \
					$(ewl_simple_test_SOURCES)

exampledir 			=	$(pkgdatadir)/examples/
example_DATA 			=	$(ewl_test_SOURCES)
