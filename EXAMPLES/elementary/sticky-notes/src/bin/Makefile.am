MAINTAINERCLEANFILES = Makefile.in

INCLUDES = \
-I$(top_srcdir) \
-I$(top_srcdir)/src/bin \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
@ELEMENTARY_CFLAGS@

bin_PROGRAMS = sticky-notes-edje

sticky_notes_edje_LDADD = @ELEMENTARY_LIBS@
sticky_notes_edje_SOURCES = db.c gui.c main.c
