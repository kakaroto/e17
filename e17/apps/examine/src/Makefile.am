## Process this file with automake to produce Makefile.in

INCLUDES = \
@ecore_cflags@ \
@libreadline_cflags@ \
@ewl_cflags@

bin_PROGRAMS = \
exsh \
examine

exsh_SOURCES = \
exsh.c \
ecore_config_client.c \
ecore_config_client.h

exsh_LDADD = \
@libreadline_libs@ \
@ecore_libs@ -ldl

exsh_LDFLAGS =

exsh_CFLAGS = \
$(CFLAGS) \
$(INCLUDES)

exsh_DEPENDENCIES =

examine_SOURCES = \
examine.c \
ecore_config_client.c \
ecore_config_client.h 

examine_LDADD = \
@ewl_libs@ -ldl

examine_LDFLAGS =

examine_CFLAGS = \
$(CFLAGS) \
$(INCLUDES) \
@ewl_cflags@

examine_DEPENDENCIES = 

