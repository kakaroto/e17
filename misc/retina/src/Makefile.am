## Process this file with automake to produce Makefile.in

INCLUDES = \
@evas_cflags@ \
@ecore_cflags@ \
@imlib2_cflags@

bin_PROGRAMS = retina

retina_SOURCES = \
r_gui.c \
r_img.c \
r_thumb.c \
r_cb.c \
retina.c

retina_LDADD = @evas_libs@ @ecore_libs@ @imlib2_libs@
