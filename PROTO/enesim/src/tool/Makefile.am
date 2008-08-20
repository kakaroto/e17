MAINTAINERCLEANFILES = \
Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir)/src/include \
-I. \
@ENESIM_CFLAGS@

noinst_PROGRAMS = enesim_generator

enesim_generator_LDADD = \
@COVERAGE_LIBS@

enesim_generator_DEPENDENCIES =
enesim_generator_SOURCES = \
enesim_generator.c \
enesim_generator_core.c \
enesim_generator_drawer.c \
common.c \
fixed.c
