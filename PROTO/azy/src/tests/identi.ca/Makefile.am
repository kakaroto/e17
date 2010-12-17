COMMON_S = \
  $(top_builddir)/src/tests/identi.ca/identica_Common.h \
  $(top_builddir)/src/tests/identi.ca/identica_Common.c

BUILT_SOURCES = \
  $(COMMON_S)

EXTRA_DIST = \
  get.c \
  identi.ca.azy

AM_CFLAGS= \
  $(AZY_CFLAGS) \
  -I$(top_srcdir) \
  -I$(top_srcdir)/src/include
AM_CXXFLAGS = $(AM_CFLAGS)

CLEANFILES = $(BUILT_SOURCES) .sources-ts
MAINTAINERCLEANFILES = Makefile.in

noinst_LTLIBRARIES = libidentica.la
nodist_libidentica_la_SOURCES = $(BUILT_SOURCES)
libidentica_la_LIBADD = \
  $(AZY_LIBS) \
  $(top_builddir)/src/lib/libazy.la


check_PROGRAMS = get
nodist_get_SOURCES = get.c
get_LDADD = \
  $(AZY_LIBS) \
  $(top_builddir)/src/lib/libazy.la \
  libidentica.la


$(BUILT_SOURCES): .sources-ts

.sources-ts: identi.ca.azy $(top_builddir)/src/bin/azy_parser
	$(top_builddir)/src/bin/azy_parser -eHn -m common-impl,common-headers -o $(top_builddir)/src/tests/identi.ca $(top_srcdir)/src/tests/identi.ca/identi.ca.azy
	touch .sources-ts
