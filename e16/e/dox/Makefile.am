SUBDIRS=E-docs

EXTRA_DIST = ChangeLog README

if FSSTD
bindir = @bindir@
ENLIGHTENMENT_BIN=$(bindir)
ENLIGHTENMENT_ROOT=$(pkgdatadir)
else
ENLIGHTENMENT_BIN=$(prefix)/enlightenment/bin
ENLIGHTENMENT_ROOT=$(prefix)/enlightenment
bindir=$(ENLIGHTENMENT_BIN)
endif

bin_PROGRAMS = dox

dox_SOURCES = \
        format.c \
        file.c \
        ttfont.c \
	text.c \
	dox.c \
	title.xpm \
	prev1.xpm \
	prev2.xpm \
	next1.xpm \
	next2.xpm \
	exit1.xpm \
	exit2.xpm \
	dox.h

LDADD = $(TTF_LIBS) $(FNLIB_LIBS) $(IMLIB_LIBS) $(XINERAMA_LIBS) -lm

INCLUDES=-I$(top_srcdir) $(IMLIB_CFLAGS) -I$(includedir) -I.. -I/usr/include/freetype1

DEFS=-DENLIGHTENMENT_ROOT=\"@ENLIGHTENMENT_ROOT@\" -DENLIGHTENMENT_BIN=\"@ENLIGHTENMENT_BIN@\"

#install-exec-local:
#	if [ x@USE_FSSTD@ = "xno" ]; then \
#	  ../mkinstalldirs $(exec_prefix)/bin; \
#	  for i in $(bin_PROGRAMS); do \
#	    rm -f $(exec_prefix)/bin/$$i; \
#	      $(LN_S) $(bindir)/$$i $(exec_prefix)/bin/$$i; \
#	  done; \
#	fi

uninstall-local:
	for i in $(bin_PROGRAMS); do \
	  rm -f $(exec_prefix)/bin/$$i; \
	done
