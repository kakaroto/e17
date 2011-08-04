
MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
@ESPIONNAGE_CFLAGS@ \
@FACE_CFLAGS@

bin_PROGRAMS = espionnage

espionnage_SOURCES = \
espionnage_main.c \
espionnage_face.cpp

espionnage_LDADD = @ESPIONNAGE_LIBS@ @LTLIBINTL@ @FACE_LIBS@

EXTRA_DIST=espionnage_private.h
