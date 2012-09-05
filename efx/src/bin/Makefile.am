MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir)/src/lib \
-I$(top_builddir)/src/lib \
-DPACKAGE_DATADIR=\"$(pkgdatadir)\" \
-DBUILD_DATADIR=\"$(top_srcdir)/data\" \
@EFX_TEST_CFLAGS@ \
@EFX_TEST_ELM_CFLAGS@

bin_PROGRAMS =

if BUILD_TESTS

bin_PROGRAMS += \
test_bumpmap \
test_fade \
test_fade2 \
test_move \
test_move_circle \
test_follow \
test_multiobject \
test_pan \
test_path \
test_presentation \
test_queue \
test_queue_complex \
test_resize \
test_rotate \
test_spin \
test_zoom \
test_zoomspin

if BUILD_TEST_ELM

bin_PROGRAMS += test_elm
test_elm_SOURCES = test_elm.c
test_elm_LDADD = $(top_builddir)/src/lib/libefx.la @EFX_TEST_ELM_LIBS@ @EFX_LIBS@

endif

test_rotate_SOURCES = test_rotate.c
test_rotate_LDADD = $(top_builddir)/src/lib/libefx.la @EFX_TEST_LIBS@

test_spin_SOURCES = test_spin.c
test_spin_LDADD = $(top_builddir)/src/lib/libefx.la @EFX_TEST_LIBS@

test_zoom_SOURCES = test_zoom.c
test_zoom_LDADD = $(top_builddir)/src/lib/libefx.la @EFX_TEST_LIBS@

test_zoomspin_SOURCES = test_zoomspin.c
test_zoomspin_LDADD = $(top_builddir)/src/lib/libefx.la @EFX_TEST_LIBS@

test_multiobject_SOURCES = test_multiobject.c
test_multiobject_LDADD = $(top_builddir)/src/lib/libefx.la @EFX_TEST_LIBS@

test_follow_SOURCES = test_follow.c
test_follow_LDADD = $(top_builddir)/src/lib/libefx.la @EFX_TEST_LIBS@

test_move_SOURCES = test_move.c
test_move_LDADD = $(top_builddir)/src/lib/libefx.la @EFX_TEST_LIBS@

test_move_circle_SOURCES = test_move_circle.c
test_move_circle_LDADD = $(top_builddir)/src/lib/libefx.la @EFX_TEST_LIBS@

test_path_SOURCES = test_path.c
test_path_LDADD = $(top_builddir)/src/lib/libefx.la @EFX_TEST_LIBS@

test_pan_SOURCES = test_pan.c
test_pan_LDADD = $(top_builddir)/src/lib/libefx.la @EFX_TEST_LIBS@

test_bumpmap_SOURCES = test_bumpmap.c
test_bumpmap_LDADD = $(top_builddir)/src/lib/libefx.la @EFX_TEST_LIBS@

test_fade_SOURCES = test_fade.c
test_fade_LDADD = $(top_builddir)/src/lib/libefx.la @EFX_TEST_LIBS@

test_fade2_SOURCES = test_fade2.c
test_fade2_LDADD = $(top_builddir)/src/lib/libefx.la @EFX_TEST_LIBS@

test_queue_SOURCES = test_queue.c
test_queue_LDADD = $(top_builddir)/src/lib/libefx.la @EFX_TEST_LIBS@

test_queue_complex_SOURCES = test_queue_complex.c
test_queue_complex_LDADD = $(top_builddir)/src/lib/libefx.la @EFX_TEST_LIBS@

test_resize_SOURCES = test_resize.c
test_resize_LDADD = $(top_builddir)/src/lib/libefx.la @EFX_TEST_LIBS@

test_presentation_SOURCES = test_presentation.c
test_presentation_LDADD = $(top_builddir)/src/lib/libefx.la @EFX_TEST_LIBS@

endif
