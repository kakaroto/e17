SUBDIRS = component 

MAINTAINERCLEANFILES = \
Makefile.in

INCLUDES = -I. \
-I$(top_srcdir)/src/lib \
@EDATA_CFLAGS@

noinst_LTLIBRARIES = libenesim_vector.la

libenesim_vector_la_SOURCES = \
container.c

libenesim_vector_la_LIBADD = \
component/libenesim_component.la

libenesim_vector_la_DEPENDENCIES = \
component/libenesim_component.la
