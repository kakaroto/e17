## Process this file with automake to produce Makefile.in

INCLUDES = \
	-I. $(top_srcdir) \
    @GTK_CFLAGS@

bin_PROGRAMS = geist

geist_SOURCES = \
	main.c \
	utils.c utils.h \
    debug.h \
    geist.h \
    layers.c layers.h \
    geist_list.c geist_list.h \
    geist_object.h geist_object.c \
    structs.h \
    geist_image.c geist_image.h \
    geist_imlib.c geist_imlib.h \
    geist_document.c geist_document.h \
    geist_gtk.c geist_gtk.h \
    imlib.c

geist_LDADD = @GTK_LIBS@ -lxml -lz -lImlib2
