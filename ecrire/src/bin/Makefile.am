
MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir)/src/lib \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
-DLOCALE_DIR=\"$(datadir)/locale\" \
@EFL_CFLAGS@ \
@ECORE_X_CFLAGS@

bin_PROGRAMS = ecrire

ecrire_SOURCES = \
   main.c \
   file_utils.c \
   cfg.c \
   ui/file_related.c \
   ui/search_dialog.c \
   ui/font_dialog.c \
   ui/alerts.c

ecrire_LDADD = @EFL_LIBS@ @ECORE_X_LIBS@ @LTLIBINTL@

EXTRA_DIST = \
   mess_header.h \
   ui/ui.h \
   cfg.h

