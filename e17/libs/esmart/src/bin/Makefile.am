MAINTAINERCLEANFILES = Makefile.in

AM_CFLAGS = -I$(top_srcdir)/src/lib \
            -I$(top_srcdir)/src/lib/esmart_thumb \
            -I$(top_srcdir)/src/lib/esmart_container \
            -I$(top_srcdir)/src/lib/esmart_trans_x11 \
            -I$(top_srcdir)/src/lib/esmart_draggies \
            -I$(top_srcdir)/src/lib/esmart_text_entry \
            -I$(top_srcdir)/src/lib/esmart_file_dialog \
            @CONTAINER_CFLAGS@ @THUMB_CFLAGS@

bin_PROGRAMS			= esmart_test esmart_file_dialog_test

#esmart_test_DEPENDENCIES	= $(top_builddir)/src/lib/libesmart.la
esmart_test_SOURCES		= esmart_test.c 

esmart_test_LDADD		= \
		$(top_builddir)/src/lib/esmart_container/libesmart_container.la \
		$(top_builddir)/src/lib/esmart_draggies/libesmart_draggies.la \
		$(top_builddir)/src/lib/esmart_thumb/libesmart_thumb.la \
		$(top_builddir)/src/lib/esmart_trans_x11/libesmart_trans_x11.la \
		@CONTAINER_LIBS@ @THUMB_LIBS@

esmart_file_dialog_test_SOURCES		= esmart_file_dialog_test.c 

esmart_file_dialog_test_LDADD = \
		$(top_builddir)/src/lib/esmart_text_entry/libesmart_text_entry.la \
		$(top_builddir)/src/lib/esmart_container/libesmart_container.la \
		$(top_builddir)/src/lib/esmart_file_dialog/libesmart_file_dialog.la \
		@FILE_DIALOG_LIBS@
