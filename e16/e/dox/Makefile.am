SUBDIRS=E-docs

EXTRA_DIST = ChangeLog README

bin_PROGRAMS = dox

dox_SOURCES = \
        format.c \
        file.c \
        ttfont.c \
	text.c \
	dox.c \
	dox.h

LDADD = $(TTF_LIBS) $(FNLIB_LIBS) $(IMLIB_LIBS) $(XINERAMA_LIBS) -lm

INCLUDES = $(IMLIB_CFLAGS) -I$(top_builddir) -I$(top_srcdir)/intl

DEFS=-DENLIGHTENMENT_ROOT=\"@ENLIGHTENMENT_ROOT@\" -DENLIGHTENMENT_BIN=\"@ENLIGHTENMENT_BIN@\"

uninstall-local:
	for i in $(bin_PROGRAMS); do \
	  rm -f $(exec_prefix)/bin/$$i; \
	done
