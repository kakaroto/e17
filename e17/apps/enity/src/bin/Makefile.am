## Process this file with automake to produce Makefile.in

INCLUDES = \
-Wall -g \
-I/usr/local/include \
-I$(top_srcdir)/lib \
@my_includes@

bin_PROGRAMS = enity

enity_INCLUDES = \
enity.h en_argument.h

enity_SOURCES = \
enity.c en_argument.c \
$(enity_INCLUDES)

enity_LDADD = @my_libs@
