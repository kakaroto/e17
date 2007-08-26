## Process this file with automake to produce Makefile.in

INCLUDES = \
	@ECORE_CFLAGS@ @ECORE_CONFIG_CFLAGS@ @EVAS_CFLAGS@ @EWL_CFLAGS@ @EDJE_CFLAGS@ @EDB_CFLAGS@ @IMLIB2_CFLAGS@ @ESMART_CFLAGS@

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
msgbox.c \
msgbox.h \
saveload.c \
saveload.h \
ipc.c \
ipc.h \
debug.c \
debug.h \
welcome.c \
welcome.h \
menu.c \
menu.h

enotes_LDADD = @EWL_LIBS@ @ECORE_LIBS@ @ECORE_CONFIG_LIBS@ @EVAS_LIBS@ @EDJE_LIBS@ @EDB_LIBS@ @IMLIB2_LIBS@ @ESMART_LIBS@
