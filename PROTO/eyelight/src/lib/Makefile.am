
MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-DPACKAGE_BIN_DIR=\"$(bindir)\" \
-DPACKAGE_LIB_DIR=\"$(libdir)\" \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
@EYELIGHT_CFLAGS@ \
@EFL_EYELIGHT_BUILD@


include_HEADERS = Eyelight.h \
		  Eyelight_Smart.h \
		  Eyelight_Edit.h \
		  eyelight_common.h

lib_LTLIBRARIES = libeyelight.la

libeyelight_la_SOURCES  = \
eyelight_viewer.c \
eyelight_viewer_expose.c \
eyelight_viewer_slideshow.c \
eyelight_viewer_gotoslide.c \
eyelight_viewer_tableofcontents.c \
eyelight_viewer_thumbnails.c \
eyelight_compiler.c \
eyelight_object.c \
eyelight_parser.c \
eyelight_smart.c \
edit/edit.c \
edit/edit_area.c \
edit/edit_slide.c \
edit/edit_image.c

libeyelight_la_LDFLAGS = -no-undefined @lt_enable_auto_import@
libeyelight_la_LIBADD = @EYELIGHT_LIBS@

EXTRA_DIST = \
eyelight_compiler_parser.h \
edit/edit.h \
eyelight_object.h \
eyelight_viewer.h \
eyelight_viewer_thumbnails.h

clean-local:
	@rm -rf *.gcno
