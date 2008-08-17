## Process this file with automake to produce Makefile.in

INCLUDES = \
@ICONBAR_CFLAGS@ 

bin_PROGRAMS = iconbar

iconbar_SOURCES = \
iconbar.c iconbar.h \
iconbar_smart.c \
icon.c icon.h \
util.c util.h \
prefs.c prefs.h \
main.c

iconbar_LDADD = \
@ICONBAR_LIBS@ \
-lesmart_container \
-lesmart_trans_x11 


