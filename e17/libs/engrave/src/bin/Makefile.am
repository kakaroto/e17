
INCLUDES = \
-I. -I$(top_srcdir)/src/lib -I$(top_srcdir)

bin_PROGRAMS = \
engrave_test

engrave_test_SOURCES = engrave_test_main.c
engrave_test_LDADD = $(top_builddir)/src/lib/libengrave.la
engrave_test_LDFLAGS = $(LDFLAGS)
engrave_test_CFLAGS = $(CFLAGS)

