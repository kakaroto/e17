SUBDIRS = rop

MAINTAINERCLEANFILES = \
Makefile.in

INCLUDES = -I. \
-I$(top_srcdir)/src/lib

noinst_LTLIBRARIES = libenesim_surface.la

libenesim_surface_la_SOURCES = \
surface.c

libenesim_surface_la_LIBADD = \
rop/libenesim_surface_rop.la

libenesim_surface_la_DEPENDENCIES = \
rop/libenesim_surface_rop.la

