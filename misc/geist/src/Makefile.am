## Process this file with automake to produce Makefile.in

SUBDIRS = testimages ttfonts

INCLUDES = \
	-I. -I$(top_srcdir) \
    @GTK_CFLAGS@ -DPREFIX=\""$(prefix)"\"

bin_PROGRAMS = geist

geist_SOURCES = \
	main.c \
	utils.c utils.h \
    debug.h \
    geist.h \
    geist_layer.c geist_layer.h \
    geist_list.c geist_list.h \
    geist_object.h geist_object.c \
    structs.h \
    geist_image.c geist_image.h \
    geist_text.c geist_text.h \
    geist_rect.c geist_rect.h \
    geist_imlib.c geist_imlib.h \
    geist_document.c geist_document.h \
    geist_document_gtk.c geist_document_gtk.h \
    geist_fill.c geist_fill.h \
    imlib.c

dnl geist_LDADD = @GTK_LIBS@ -lxml -lz -lImlib2
geist_LDADD = @GTK_LIBS@ -lImlib2
