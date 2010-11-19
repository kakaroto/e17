MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir) \
-I$(top_srcdir)/bin \
-I$(top_srcdir)/src/lib \
-DPACKAGE_BIN_DIR=\"$(bindir)\" \
-DPACKAGE_LIB_DIR=\"$(libdir)\" \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
@ESKISS_CFLAGS@

bin_PROGRAMS = eskiss

eskiss_SOURCES = \
draw_object.c \
game.c \
level.c \
level_chooser.c \
level_editor.c \
main.c \
physic_object.c

eskiss_LDADD = @ESKISS_LIBS@
eskiss_LDFLAGS = @lt_enable_auto_import@ @ld_as_needed@
