## Process this file with automake to produce Makefile.in

SUBDIRS = testimages ttfonts

INCLUDES = \
-I. -I$(top_srcdir) \
@GTK_CFLAGS@ -DPREFIX=\""$(prefix)"\" @XML2_CFLAGS@ @GIBLIB_CFLAGS@

bin_PROGRAMS = geist

testdocs_DATA = test.xml text_test.xml
testdocsdir=$(prefix)/share/geist

EXTRA_DIST = $(testdocs_DATA)

geist_SOURCES = \
	main.c \
	utils.c utils.h \
    debug.h \
    geist.h \
    geist_layer.c geist_layer.h \
    geist_object.h geist_object.c \
    structs.h \
    geist_image.c geist_image.h \
    geist_text.c geist_text.h \
    geist_rect.c geist_rect.h \
    geist_line.c geist_line.h \
    geist_poly.c geist_poly.h \
    geist_imlib.c geist_imlib.h \
    geist_document.c geist_document.h \
    geist_document_gtk.c geist_document_gtk.h \
    geist_fill.c geist_fill.h \
    geist_document_xml.c geist_document_xml.h \
    geist_gtk_menu.c geist_gtk_menu.h \
    geist_interface.c geist_interface.h \
    geist_utils.c geist_utils.h \
    imlib.c

geist_LDADD = @GTK_LIBS@ @GIBLIB_LIBS@ @XML2_LIBS@
