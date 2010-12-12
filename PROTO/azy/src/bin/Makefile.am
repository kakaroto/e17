SUBDIRS = re2c

BUILT_SOURCES = \
	$(top_builddir)/src/bin/re2c/re2c \
	azy_parser.h \
	azy_parser.c

MAINTAINERCLEANFILES = Makefile.in
CLEANFILES = \
	azy_parser.h \
	azy_parser.c \
	azy_parser.y

EXTRA_DIST = \
	lempar.c \
	lemon.c \
	azy_parser.1 \
	azy_parser.yre \
	$(BUILT_SOURCES)

AM_CFLAGS = \
	$(AZY_CFLAGS)

LDADD = \
	$(AZY_LIBS)

man_MANS = azy_parser.1
bin_PROGRAMS = azy_parser

azy_parser_SOURCES = \
	azy.h \
	azy.c \
	main.c \
	parser_lib.h \
	parser_lib.c \
	azy_parser.h \
	azy_parser.c

noinst_PROGRAMS = lemon
lemon_SOURCES = lemon.c

# parser

$(top_builddir)/src/bin/re2c/re2c:
	cd $(top_builddir)/src/bin/re2c && $(MAKE) $(AM_MAKEFLAGS)
azy_parser.h: azy_parser.c $(top_builddir)/src/bin/re2c/re2c
azy_parser.c: azy_parser.yre $(top_builddir)/src/bin/re2c/re2c lemon
	@if test -x $(top_srcdir)/src/bin/re2c/re2c -a "x$(top_srcdir)" != "x$(top_builddir)"; then \
		cp $(top_srcdir)/src/bin/re2c/re2c $(top_builddir)/src/bin/re2c; \
	fi
	$(top_builddir)/src/bin/re2c/re2c -s -i -o azy_parser.y $<
	@if test "x$(top_srcdir)" != "x$(top_builddir)"; then \
		cp $(top_srcdir)/src/bin/lempar.c $(top_builddir)/src/bin; \
	fi
	./lemon -q azy_parser.y

clean-local:
	@if test "x$(top_srcdir)" != "x$(top_builddir)"; then \
		rm -f $(top_builddir)/src/bin/lempar.c; \
	fi
