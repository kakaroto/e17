
MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir)/src/lib/esmart_dialog \
@FILE_DIALOG_CFLAGS@ @EINA_CFLAGS@

pkgdir = @libdir@
pkg_LTLIBRARIES = libesmart_file_dialog.la

installed_headersdir = $(prefix)/include/Esmart
installed_headers_DATA = Esmart_File_Dialog.h

libesmart_file_dialog_la_DEPENDENCIES = \
$(top_builddir)/src/lib/esmart_container/libesmart_container.la \
$(top_builddir)/src/lib/esmart_text_entry/libesmart_text_entry.la

libesmart_file_dialog_la_SOURCES = \
esmart_file_dialog.c \
Esmart_File_Dialog.h

libesmart_file_dialog_la_LDFLAGS = -version-info @version_info@
libesmart_file_dialog_la_LIBADD = \
$(top_builddir)/src/lib/esmart_container/libesmart_container.la \
$(top_builddir)/src/lib/esmart_text_entry/libesmart_text_entry.la \
@FILE_DIALOG_LIBS@ @EINA_LIBS@
