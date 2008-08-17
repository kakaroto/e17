
MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir)/src/lib \
-I$(top_srcdir)/src/include \
@EVAS_CFLAGS@ \
@ECORE_CFLAGS@ \
@EDJE_CFLAGS@

bin_PROGRAMS = epsilon epsilon_thumbd epsilon_thumb_test

epsilon_SOURCES = \
epsilon_main.c

epsilon_LDADD = \
$(top_builddir)/src/lib/libepsilon.la \
@EDJE_LIBS@ \
@ECORE_LIBS@ \
@EVAS_LIBS@

epsilon_DEPENDENCIES = $(top_builddir)/src/lib/libepsilon.la

epsilon_thumbd_SOURCES = \
epsilon_thumbd.c \
$(top_builddir)/src/common/epsilon_thumb_common.c

epsilon_thumbd_DEPENDENCIES = $(top_builddir)/src/lib/libepsilon.la

epsilon_thumbd_LDADD = \
$(top_builddir)/src/lib/libepsilon.la \
@ECORE_LIBS@

epsilon_thumb_test_SOURCES = \
epsilon_thumb_test.c

epsilon_thumb_test_DEPENDENCIES = $(top_builddir)/src/lib/libepsilon.la

epsilon_thumb_test_LDADD = \
$(top_builddir)/src/lib/libepsilon.la \
@ECORE_LIBS@
