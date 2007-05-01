## Process this file with automake to produce Makefile.in

AM_CFLAGS = \
@ECORE_CFLAGS@ \
@libreadline_cflags@ \
@EWL_CFLAGS@

bin_PROGRAMS = \
exsh \
examine

exsh_SOURCES = \
exsh.c \
ecore_config_client.c \
ecore_config_client.h

exsh_LDADD = \
@libreadline_libs@ \
@ECORE_LIBS@

exsh_LDFLAGS =

exsh_DEPENDENCIES =

examine_SOURCES = \
ipc.h \
examine_client.c \
examine_client.h \
ecore_config_client.c \
ecore_config_client.h \
examine.c

examine_LDADD = \
@EWL_LIBS@ \
@ECORE_LIBS@

examine_LDFLAGS =

examine_CFLAGS = \
@EWL_CFLAGS@

examine_DEPENDENCIES = 
