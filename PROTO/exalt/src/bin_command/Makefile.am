## Process this file with automake to produce Makefile.in

AM_CFLAGS = \
-Wall -W -g -D_GNU_SOURCE @EXALT_CFLAGS@ \
-I$(top_srcdir)/src/lib_dbus  -I$(top_srcdir)/src/lib


sbin_PROGRAMS = exalt-command

exalt_command_INCLUDES = \
command.h

exalt_command_SOURCES = \
command.c

exalt_command_LDFLAGS = @DLLDFLAGS@
exalt_command_LDADD = $(top_builddir)/src/lib_dbus/libexalt_dbus.la $(top_builddir)/src/lib/libexalt.la @EXALT_LIBS@

MAINTAINERCLEANFILES = Makefile.in
