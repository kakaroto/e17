## Process this file with automake to produce Makefile.in

INCLUDES = \
-Wall -g \
-I$(top_srcdir)/lib \
@exhibit_cflags@ \
@optional_cflags@

bin_PROGRAMS = exhibit

exhibit_INCLUDES = \
exhibit.h \
exhibit_file.h \
exhibit_image.h \
exhibit_main.h \
exhibit_menus.h \
exhibit_sort.h \
exhibit_thumb.h \
exhibit_tab.h \
exhibit_slideshow.h \
exhibit_favorites.h \
exhibit_options.h \
exhibit_comment.h \
exhibit_comment_jpeg.h \
exhibit_model_wobbly.h

exhibit_SOURCES = \
exhibit_file.c \
exhibit_image.c \
exhibit_main.c \
exhibit_menus.c \
exhibit_sort.c \
exhibit_thumb.c \
exhibit_tab.c	\
exhibit_slideshow.c \
exhibit_favorites.c \
exhibit_options.c \
exhibit_comment.c \
exhibit_comment_jpeg.c \
exhibit_model_wobbly.c \
$(exhibit_INCLUDES)

exhibit_LDADD = @exhibit_libs@ @optional_libs@
