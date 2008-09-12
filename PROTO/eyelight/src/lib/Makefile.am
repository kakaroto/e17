
MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I. \
-I$(top_srcdir)/src/lib \
-DPACKAGE_BIN_DIR=\"$(bindir)\" \
-DPACKAGE_LIB_DIR=\"$(libdir)\" \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\"

INCLUDES = \
-I. \
-I$(top_srcdir)/src/lib \
-DPACKAGE_BIN_DIR=\"$(bindir)\" \
-DPACKAGE_LIB_DIR=\"$(libdir)\" \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
@eyelight_cflags@


include_HEADERS = Eyelight.h \
		  eyelight_compiler_common.h \
		  eyelight_compiler.h \
		  eyelight_parser.h \
		  eyelight_viewer_thumbnails.h \
		  eyelight_viewer.h

lib_LTLIBRARIES = libeyelight.la

libeyelight_la_SOURCES  = \
eyelight_viewer.c \
eyelight_viewer_expose.c \
eyelight_viewer_slideshow.c \
eyelight_viewer_gotoslide.c \
eyelight_viewer_tableofcontents.c \
eyelight_viewer_thumbnails.c \
eyelight_compiler_common.c \
eyelight_compiler.c \
eyelight_parser.c

libeyelight_la_DEPENDENCIES = $(top_builddir)/config.h
libeyelight_la_LDFLAGS = @eyelight_libs@

clean-local:
	@rm -rf *.gcno
