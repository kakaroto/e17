AUTOMAKE_OPTIONS     = 1.4 foreign

MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/src/bin @EVAS_CFLAGS@ @ECORE_CFLAGS@ @EDJE_CFLAGS@ @EMOTION_CFLAGS@

bin_PROGRAMS = rage rage_thumb raged

rage_SOURCES = \
main.c \
main.h \
layout.c \
layout.h \
background.c \
background.h \
volume.c \
volume.h \
status.c \
status.h \
menu.c \
menu.h \
video.c \
video.h \
mini.c \
mini.h \
sha1.c \
sha1.h \
minivid.c \
minivid.h \
e.h \
e_box.c \
e_box.h \
e_table.c \
e_table.h \
e_layout.c \
e_layout.h \
e_flowlayout.c \
e_flowlayout.h \
conf_options.c \
conf_options.h \
dvb.c \
dvb.h

rage_LDADD = @my_libs@ @EVAS_LIBS@ @ECORE_LIBS@ @EDJE_LIBS@ @EMOTION_LIBS@

rage_thumb_SOURCES = \
rage_thumb.c

rage_thumb_LDADD = @my_libs@ @EVAS_LIBS@ @ECORE_LIBS@ @EDJE_LIBS@ @EMOTION_LIBS@

raged_SOURCES = \
raged_main.c \
sha1.c \
sha1.h

raged_LDADD = @my_libs@ @EVAS_LIBS@ @ECORE_LIBS@ @EMOTION_LIBS@ @EET_LIBS@
