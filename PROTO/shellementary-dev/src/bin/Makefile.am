AUTOMAKE_OPTIONS     = 1.4 foreign

MAINTAINERCLEANFILES = Makefile.in

INCLUDES = \
-I$(top_srcdir) \
-I$(top_srcdir)/src/bin \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
@EINA_CFLAGS@ @ECORE_CFLAGS@ @ELEMENTARY_CFLAGS@

bin_PROGRAMS = shellementary

shellementary_SOURCES = shelm.c \
			shelm.h \
			shelm_getopt.h \
			\
			shelm_macros.c \
			shelm_callbacks.c \
			\
			shelm_dlg_entry.c \
			shelm_dlg_simpleicon.c \
			shelm_dlg_question.c \
			shelm_dlg_scale.c \
			shelm_dlg_clock.c \
			shelm_dlg_textinfo.c \
			shelm_dlg_list.c \
			\
			shelm_about.c

shellementary_LDADD = @EINA_LIBS@ @ECORE_LIBS@ @ELEMENTARY_LIBS@
