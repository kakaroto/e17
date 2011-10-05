
MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I. \
-I$(top_srcdir)/lib \
-I$(top_builddir)/lib \
-DPACKAGE_PKGDATA_DIR=\"$(pkgdatadir)\" \
@EQUATE_CFLAGS@

bin_PROGRAMS = equate

equate_SOURCES = \
gui.c \
calc_parse.c \
edje.c \
main.c \
Equate.h \
calc.h \
gui.h

equate_LDADD = @EQUATE_LIBS@

EXTRA_DIST = calc_lex.c calc.y regen.sh