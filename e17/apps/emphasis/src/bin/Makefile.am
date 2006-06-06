## Process this file with automake to produce Makefile.in

AM_CFLAGS = \
-Wall -g -Wno-unused -D_GNU_SOURCE

bin_PROGRAMS = emphasis

emphasis_INCLUDES = \
emphasis.h \
emphasis_mpc.h \
emphasis_config.h \
emphasis_gui.h \
emphasis_callbacks.h \
emphasis_misc.h

emphasis_SOURCES = \
emphasis.c \
emphasis_mpc.c \
emphasis_config.c \
emphasis_gui.c \
emphasis_callbacks.c \
emphasis_misc.c 

emphasis_LDADD = @my_libs@ -lmpd
