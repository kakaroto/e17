MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir)/src/lib \
-I$(top_builddir)/
AM_CFLAGS = @EFX_DEPS_CFLAGS@ @ECORE_EVAS_CFLAGS@

LDADD = $(top_builddir)/src/lib/libefx.la @ECORE_EVAS_LIBS@ @EFX_DEPS_LIBS@

if BUILD_TESTS

bin_PROGRAMS = \
test_rotate \
test_spin \
test_zoom \
test_zoomspin \
test_multiobject \
test_follow \
test_move \
test_path

if BUILD_TEST_ELM

bin_PROGRAMS += test_elm
test_elm_SOURCES = test_elm.c
test_elm_CFLAGS = $(AM_CFLAGS) @ELM_CFLAGS@
test_elm_LDADD = $(LDADD) @ELM_LIBS@

endif

test_rotate_SOURCES = test_rotate.c
test_spin_SOURCES = test_spin.c
test_zoom_SOURCES = test_zoom.c
test_zoomspin_SOURCES = test_zoomspin.c
test_multiobject_SOURCES = test_multiobject.c
test_follow_SOURCES = test_follow.c
test_move_SOURCES = test_move.c
test_path_SOURCES = test_path.c

endif
