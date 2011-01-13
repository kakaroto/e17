if NEED_RE2C
SUBDIRS = re2c
REC = $(top_srcdir)/src/bin/re2c/re2c
else
REC = $(RE2C)
endif

RE2C_OPTS = -bi -o azy_parser.y

BUILT_SOURCES = \
	azy_parser.c \
	azy_parser.y

MAINTAINERCLEANFILES = Makefile.in
CLEANFILES = \
	azy_parser.h \
	$(BUILT_SOURCES)

EXTRA_DIST = \
	lempar.c \
	lemon.c \
	azy_parser.yre

AM_CFLAGS = \
	$(AZY_CFLAGS)

LDADD = \
	$(AZY_LIBS)

bin_PROGRAMS = azy_parser

azy_parser_SOURCES = \
	azy.h \
	azy.c \
	main.c \
	azy_parser_lib.h \
	azy_parser_lib.c \
	azy_parser.c

noinst_PROGRAMS = lemon
lemon_SOURCES = lemon.c

# parser

if NEED_RE2C
$(REC):
	cd re2c && $(MAKE) $(AM_MAKEFLAGS)
else
.PHONY: $(REC)
endif
azy_parser.y: azy_parser.yre $(REC) lemon
	$(REC) $(RE2C_OPTS) $<

azy_parser.c: azy_parser.y lemon
	@if test "x$(top_srcdir)" != "x$(top_builddir)"; then \
		cp $(top_srcdir)/src/bin/lempar.c $(top_builddir)/src/bin; \
	fi
	./lemon -q $<

clean-local:
	@if test "x$(top_srcdir)" != "x$(top_builddir)"; then \
		rm -f $(top_builddir)/src/bin/lempar.c; \
	fi
