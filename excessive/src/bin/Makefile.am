
MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
@EXCESSIVE_CFLAGS@

bin_PROGRAMS = excessive

excessive_SOURCES = \
excessive_main.c \
excessive_browse_directory.c \
excessive_show_dir.c \
excessive_show_emotion.c \
excessive_show_image.c \
excessive_shortcut.c

excessive_LDADD = @EXCESSIVE_LIBS@ @LTLIBINTL@

