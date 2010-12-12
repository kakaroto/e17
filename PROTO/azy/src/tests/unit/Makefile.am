EXTRA_DIST = \
  tests.h

check_PROGRAMS = t001_content

t001_content_CFLAGS = \
  $(AZY_CFLAGS) \
  -I$(top_builddir) \
  -I$(top_srcdir)/src/include \
  -I$(top_srcdir)/src/include/extras

t001_content_SOURCES = \
  t001_content.c

t001_content_LDADD = \
  $(AZY_LIBS) \
  $(top_builddir)/src/lib/libazy.la

##t001_content_LDFLAGS = -static

MAINTAINERCLEANFILES = Makefile.in
