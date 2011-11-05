
MAINTAINERCLEANFILES = Makefile.in

#bin_PROGRAMS = epdf_evas_test epdf_esmart_test
bin_PROGRAMS = epdf_evas_test

## evas test

epdf_evas_test_SOURCES = epdf_evas_test.c

epdf_evas_test_CPPFLAGS =\
-I$(top_srcdir)/src/lib	\
@EPDFBIN_CFLAGS@ \
@EPDF_CFLAGS@

epdf_evas_test_LDADD = \
$(EPDFBIN_LIBS) \
$(top_builddir)/src/lib/libepdf.la

## esmart test

epdf_esmart_test_SOURCES = epdf_esmart_test.c

epdf_esmart_test_CPPFLAGS =\
-I$(top_srcdir)/src/lib	\
@EPDFBIN_CFLAGS@ \
@EPDF_CFLAGS@

epdf_esmart_test_LDADD = \
$(EPDFBIN_LIBS) \
$(top_builddir)/src/lib/libepdf.la
