pkgdir = @libdir@/@PACKAGE@
pkg_LTLIBRARIES = libesmart_file_dialog.la

INCLUDES = -I. \
			  @imlib_libs@ \
			  @evas_libs@ \
			  @edje_cflags@

installed_headersdir = @prefix@/include/Esmart
installed_headers_DATA = Esmart_File_Dialog.h

#libesmart_file_dialog_la_DEPENDENCIES = \
#			$(top_builddir)/src/lib/esmart_container/libesmart_container.la \
#			$(top_builddir)/src/lib/esmart_text_entry/libesmart_text_entry.la

libesmart_file_dialog_la_SOURCES = esmart_file_dialog.c \
											Esmart_File_Dialog.h

libesmart_file_dialog_la_LDFLAGS = -shared -version-info 0:9:0
libesmart_file_dialog_la_LIBADD = \
					-L../esmart_container -lesmart_container \
					-L../esmart_text_entry -lesmart_text_entry \
					@imlib_libs@ \
					@evas_libs@ \
					@edje_libs@

