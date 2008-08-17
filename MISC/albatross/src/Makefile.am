## Process this file with automake to produce Makefile.in

SUBDIRS = ttfonts

INCLUDES = \
-I. -I$(top_srcdir) \
@GTK_CFLAGS@ -DPREFIX=\""$(prefix)"\" @XML2_CFLAGS@ @GIBLIB_CFLAGS@

bin_PROGRAMS = albatross

testdocs_DATA = test.xml text_test.xml
testdocsdir=$(prefix)/share/albatross

EXTRA_DIST = $(testdocs_DATA)

albatross_SOURCES = \
	main.c \
	utils.c utils.h \
  debug.h \
  alb.h \
  alb_layer.c alb_layer.h \
  alb_object.h alb_object.c \
  structs.h \
  alb_image.c alb_image.h \
  alb_thumb.c alb_thumb.h \
  alb_text.c alb_text.h \
  alb_rect.c alb_rect.h \
  alb_line.c alb_line.h \
  alb_poly.c alb_poly.h \
  alb_imlib.c alb_imlib.h \
  alb_document.c alb_document.h \
  alb_document_gtk.c alb_document_gtk.h \
  alb_fill.c alb_fill.h \
  alb_document_xml.c alb_document_xml.h \
  alb_gtk_menu.c alb_gtk_menu.h \
  alb_interface.c alb_interface.h \
  alb_utils.c alb_utils.h \
  imlib.c

albatross_LDADD = @GTK_LIBS@ @GIBLIB_LIBS@ @XML2_LIBS@
