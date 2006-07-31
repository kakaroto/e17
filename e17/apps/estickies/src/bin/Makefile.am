## Process this file with automake to produce Makefile.in
MAINTAINERCLEANFILES = Makefile.in

INCLUDES = \
-Wall \
-I/usr/local/include \
-I$(top_srcdir)/lib \
@my_includes@

bin_PROGRAMS = estickies

estickies_INCLUDES = \
stickies.h \
conf.h \
theme_chooser.h

estickies_SOURCES = \
stickies.c \
conf.c \
theme_chooser.c \
$(estickies_INCLUDES)

estickies_LDADD = @my_libs@
