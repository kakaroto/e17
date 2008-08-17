## Process this file with automake to produce Makefile.in
MAINTAINERCLEANFILES = Makefile.in

INCLUDES = \
-Wall \
@ESTICKIES_CFLAGS@

bin_PROGRAMS = estickies

estickies_INCLUDES = \
stickies.h \
conf.h \
theme_chooser.h \
about.h \
plugin.h

estickies_SOURCES = \
stickies.c \
conf.c \
theme_chooser.c \
about.c \
plugin.c \
$(estickies_INCLUDES)

estickies_LDADD = @ESTICKIES_LIBS@
