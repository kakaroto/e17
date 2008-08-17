## Process this file with automake to produce Makefile.in

MAINTAINERCLEANFILES = Makefile.in etk_server_valid_funcs.c \
                       etk_server_valid_signals.c

INCLUDES = \
-Wall -export-dynamic \
-I$(top_srcdir)/lib \
@my_includes@

bin_PROGRAMS = etk_server

etk_server_INCLUDES = \
etk_server.h

etk_server_SOURCES = \
etk_server.c \
etk_server_valid_funcs.c \
etk_server_valid_signals.c \
$(etk_server_INCLUDES)

etk_server_LDADD = @my_libs@ 
etk_server_LDFLAGS = -export-dynamic
