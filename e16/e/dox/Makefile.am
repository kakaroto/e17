SUBDIRS = E-docs

EXTRA_DIST = README

bin_PROGRAMS = edox

edox_SOURCES = dox.c dox.h file.c format.c text.c ttfont.c

LDADD = $(IMLIB_LIBS) $(X_LIBS) $(XINERAMA_LIBS) -lX11 -lm

INCLUDES = -I$(top_builddir) -I$(top_srcdir)/intl $(IMLIB_CFLAGS) $(X_CFLAGS)

uninstall-local:
	for i in $(bin_PROGRAMS); do \
	  rm -f $(exec_prefix)/bin/$$i; \
	done
