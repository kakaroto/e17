## Process this file with automake to produce Makefile.in

INCLUDES = \
-I/usr/local/include \
-I$(top_srcdir)/lib \
@my_includes@

bin_PROGRAMS = eclips

eclips_SOURCES = \
bg.c \
bg.h \
eclipse.c \
eclipse.h \
getopts.c \
getopts.h \
help.c \
help.h \
images.c \
images.h \
window.c \
window.h \
keys.c \
keys.h \
menu.c \
menu.h \
file.c \
file.h 

eclips_LDADD = @my_libs@
