## Process this file with automake to produce Makefile.in

INCLUDES = \
	@ecore_cflags@ @evas_cflags@ @ewl_cflags@ @edje_cflags@ @edb_cflags@ @imlib2_cflags@ @libxml2_cflags@ @esmart_cflags@

bin_PROGRAMS = enotes

enotes_SOURCES = \
main.c \
main.h \
config.c \
config.h \
note.c \
note.h \
controlcentre.c \
controlcentre.h \
storage.c \
storage.h \
usage.c \
usage.h \
msgbox.c \
msgbox.h \
saveload.c \
saveload.h \
settings.h \
settings.c \
ipc.c \
ipc.h \
xml.c \
xml.h \
debug.c \
debug.h

enotes_LDADD = @ewl_libs@ @ecore_libs@ @evas_libs@ @edje_libs@ @edb_libs@ @imlib2_libs@ @libxml2_libs@ @esmart_libs@
