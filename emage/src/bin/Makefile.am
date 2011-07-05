
MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
@EMAGE_CFLAGS@

bin_PROGRAMS = emage

emage_SOURCES = \
emage_main.c \
emage_browse_directory.c \
emage_show_dir.c \
emage_show_emotion.c \
emage_show_image.c \
emage_shortcut.c

emage_LDADD = @EMAGE_LIBS@ @LTLIBINTL@

