## Process this file with automake to produce Makefile.in

INCLUDES = \
-Wall -g \
-I/usr/local/include \
-I$(top_srcdir)/lib \
@my_includes@

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
exhibit_slideshow.h

exhibit_SOURCES = \
exhibit_file.c \
exhibit_image.c \
exhibit_main.c \
exhibit_menus.c \
exhibit_sort.c \
exhibit_thumb.c \
exhibit_tab.c \
exhibit_slideshow.c \
$(exhibit_includes)

exhibit_LDADD = @my_libs@
