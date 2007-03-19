AUTOMAKE_OPTIONS     = 1.4 foreign

MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/src/bin @my_cflags@

bin_PROGRAMS = rage rage_thumb

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
e_flowlayout.h

rage_LDADD = @my_libs@

rage_thumb_SOURCES = \
rage_thumb.c

rage_thumb_LDADD = @my_libs@
