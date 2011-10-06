
MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I. \
-I$(top_srcdir)/lib \
-I$(top_builddir)/lib \
-DPACKAGE_DATA_DIR=\"$(pkgdatadir)\" \
-DPACKAGE_BIN_DIR=\"$(pkgbindir)\" \
@EQUATE_CFLAGS@

bin_PROGRAMS = equate

equate_SOURCES = \
calc_parse.c \
main.c \
Equate.h \
calc.h

equate_LDADD = @EQUATE_LIBS@

EXTRA_DIST = calc_lex.c calc.y regen.sh