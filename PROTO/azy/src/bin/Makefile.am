SUBDIRS = re2c

BUILT_SOURCES = \
	azy_parser.h \
	azy_parser.c

EXTRA_DIST = \
	lempar.c \
	lemon.c \
	azy_parser.1 \
	azy_parser.yre \
	$(BUILT_SOURCES)

AM_CFLAGS = \
	$(ECORE_CFLAGS)

LDADD = \
	$(ECORE_LIBS)

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

BUILT_SOURCES += re2c/re2c

# parser

re2c/re2c:
	cd re2c && $(MAKE) $(AM_MAKEFLAGS)
azy_parser.h: azy_parser.c
azy_parser.c: azy_parser.yre re2c/re2c
	test -r lempar.c || test x$(top_srcdir)/bin = x$(top_builddir)/bin || cat lempar.c > lempar.c
	gcc -o lemon lemon.c
	re2c/re2c -s -i -o azy_parser.y $<
	./lemon -q azy_parser.y
	test x$(top_srcdir)/bin = x$(top_builddir)/bin || rm -f lempar.c

CLEANFILES = lemon azy_parser.y
MAINTAINERCLEANFILES = Makefile.in
