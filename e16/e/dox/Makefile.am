
EXTRA_DIST = ChangeLog README

bin_PROGRAMS = edox

edox_SOURCES = dox.c dox.h file.c format.c text.c ttfont.c

LDADD = $(TTF_LIBS) $(FNLIB_LIBS) $(IMLIB_LIBS) $(X_LIBS) $(XINERAMA_LIBS) -lX11 -lm

INCLUDES = $(IMLIB_CFLAGS) -I$(top_builddir) -I$(top_srcdir)/intl

DEFS=-DENLIGHTENMENT_ROOT=\"@ENLIGHTENMENT_ROOT@\" -DENLIGHTENMENT_BIN=\"@ENLIGHTENMENT_BIN@\"

uninstall-local:
	for i in $(bin_PROGRAMS); do \
	  rm -f $(exec_prefix)/bin/$$i; \
	done
