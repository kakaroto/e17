
MAINTAINERCLEANFILES = Makefile.in

if BUILD_EPS_EWL
EPS_EWL_TEST = eps_ewl_test
endif

if BUILD_EPS_ETK
EPS_ETK_TEST = eps_etk_test
endif

bin_PROGRAMS = eps_evas_test eps_esmart_test $(EPS_EWL_TEST) $(EPS_ETK_TEST)

## evas test

eps_evas_test_SOURCES = \
eps_evas_test.c

eps_evas_test_CPPFLAGS = \
-I$(top_srcdir)/src/lib \
@EPS_BIN_CFLAGS@ \
@EPS_CFLAGS@

eps_evas_test_LDADD = \
$(top_builddir)/src/lib/libeps.la \
$(EPS_LIBS) \
$(EPS_BIN_LIBS)

eps_evas_test_DEPENDENCIES = $(top_builddir)/src/lib/libeps.la

## esmart test

eps_esmart_test_SOURCES = \
eps_esmart_test.c

eps_esmart_test_CPPFLAGS = \
-I$(top_srcdir)/src/lib \
@EPS_BIN_CFLAGS@ \
@EPS_CFLAGS@

eps_esmart_test_LDADD = \
$(top_builddir)/src/lib/libeps.la \
$(EPS_LIBS) \
$(EPS_BIN_LIBS)

eps_esmart_test_DEPENDENCIES = $(top_builddir)/src/lib/libeps.la

## ewl test

eps_ewl_test_SOURCES = \
eps_ewl_test.c

eps_ewl_test_CPPFLAGS = \
-I$(top_srcdir)/src/lib \
@EPS_BIN_CFLAGS@ \
@EPS_CFLAGS@ \
@EWL_CFLAGS@

eps_ewl_test_LDADD = \
$(top_builddir)/src/lib/libeps.la \
$(EPS_BIN_LIBS) \
$(EPS_LIBS) \
$(EWL_LIBS)

eps_ewl_test_DEPENDENCIES = $(top_builddir)/src/lib/libeps.la

## etk test

eps_etk_test_SOURCES = \
eps_etk_test.c

eps_etk_test_CPPFLAGS = \
-I$(top_srcdir)/src/lib \
@EPS_BIN_CFLAGS@ \
@EPS_CFLAGS@ \
@ETK_CFLAGS@

eps_etk_test_LDADD = \
$(top_builddir)/src/lib/libeps.la \
$(EPS_BIN_LIBS) \
$(EPS_LIBS) \
$(ETK_LIBS)

eps_etk_test_DEPENDENCIES = $(top_builddir)/src/lib/libeps.la
