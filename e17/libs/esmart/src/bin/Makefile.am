
MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir)/src/lib \
-I$(top_srcdir)/src/lib/esmart_thumb \
-I$(top_srcdir)/src/lib/esmart_container \
-I$(top_srcdir)/src/lib/esmart_trans_x11 \
-I$(top_srcdir)/src/lib/esmart_draggies \
-I$(top_srcdir)/src/lib/esmart_text_entry \
-I$(top_srcdir)/src/lib/esmart_file_dialog \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
@CONTAINER_CFLAGS@ \
@THUMB_CFLAGS@

bin_PROGRAMS = esmart_test esmart_text_entry_test

#esmart_test_DEPENDENCIES = $(top_builddir)/src/lib/libesmart.la
esmart_test_SOURCES = esmart_test.c

esmart_test_LDADD = \
$(top_builddir)/src/lib/esmart_container/libesmart_container.la \
$(top_builddir)/src/lib/esmart_draggies/libesmart_draggies.la \
$(top_builddir)/src/lib/esmart_thumb/libesmart_thumb.la \
$(top_builddir)/src/lib/esmart_trans_x11/libesmart_trans_x11.la \
@CONTAINER_LIBS@ \
@THUMB_LIBS@

esmart_text_entry_test_SOURCES = esmart_text_entry_test.c

esmart_text_entry_test_LDADD = \
$(top_builddir)/src/lib/esmart_text_entry/libesmart_text_entry.la \
$(top_builddir)/src/lib/esmart_container/libesmart_container.la \
@TEXT_ENTRY_LIBS@
