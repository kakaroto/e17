
MAINTAINERCLEANFILES = Makefile.in

if BUILD_EDVI_EWL
EDVI_EWL_TEST = edvi_ewl_test
endif

if BUILD_EDVI_ETK
EDVI_ETK_TEST = edvi_etk_test
endif

bin_PROGRAMS = edvi_evas_test edvi_esmart_test $(EDVI_EWL_TEST) $(EDVI_ETK_TEST)

## evas test

edvi_evas_test_SOURCES = \
edvi_evas_test.c

edvi_evas_test_CPPFLAGS = \
-I$(top_srcdir)/src/lib	\
@ECORE_EVAS_CFLAGS@ \
@EDVI_CFLAGS@

edvi_evas_test_LDADD = \
$(top_builddir)/src/lib/libedvi.la \
$(EDVI_LIBS) $(ECORE_EVAS_LIBS)

edvi_evas_test_DEPENDENCIES = $(top_builddir)/src/lib/libedvi.la

## esmart test

edvi_esmart_test_SOURCES = \
edvi_esmart_test.c

edvi_esmart_test_CPPFLAGS = \
-I$(top_srcdir)/src/lib	\
@ECORE_EVAS_CFLAGS@ \
@EDVI_CFLAGS@

edvi_esmart_test_LDADD = \
$(top_builddir)/src/lib/libedvi.la \
$(EDVI_LIBS) $(ECORE_EVAS_LIBS)

edvi_esmart_test_DEPENDENCIES = $(top_builddir)/src/lib/libedvi.la

## ewl test

edvi_ewl_test_SOURCES = \
edvi_ewl_test.c

edvi_ewl_test_CPPFLAGS = \
-I$(top_srcdir)/src/lib	\
@ECORE_EVAS_CFLAGS@ \
@EDVI_CFLAGS@ \
@EWL_CFLAGS@

edvi_ewl_test_LDADD = \
$(top_builddir)/src/lib/libedvi.la \
$(EDVI_LIBS) \
$(EWL_LIBS)

edvi_ewl_test_DEPENDENCIES = $(top_builddir)/src/lib/libedvi.la

## etk test

edvi_etk_test_SOURCES = \
edvi_etk_test.c

edvi_etk_test_CPPFLAGS = \
-I$(top_srcdir)/src/lib	\
@ECORE_EVAS_CFLAGS@ \
@EDVI_CFLAGS@ \
@ETK_CFLAGS@

edvi_etk_test_LDADD = \
$(top_builddir)/src/lib/libedvi.la \
$(EDVI_LIBS)

edvi_etk_test_DEPENDENCIES = $(top_builddir)/src/lib/libedvi.la
