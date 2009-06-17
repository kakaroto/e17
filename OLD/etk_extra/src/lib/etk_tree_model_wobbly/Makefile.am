MAINTAINERCLEANFILES = Makefile.in

pkgdir = @libdir@

pkg_LTLIBRARIES = libetk_tree_model_wobbly.la

INCLUDES = -I$(top_srcdir)/src/lib/etk_tree_model_wobbly \
			  @ETK_CFLAGS@ \
			  @EMOTION_CFLAGS@

installed_headersdir = $(prefix)/include/etk_extra
installed_headers_DATA = etk_tree_model_wobbly.h

libetk_tree_model_wobbly_la_SOURCES = etk_tree_model_wobbly.c \
			  etk_tree_model_wobbly.h

libetk_tree_model_wobbly_la_LDFLAGS = -version-info 0:9:0
libetk_tree_model_wobbly_la_LIBADD = @ETK_LIBS@
