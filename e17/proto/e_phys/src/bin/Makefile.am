MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I$(top_srcdir)/src/lib \
@EVAS_CFLAGS@ \
@ECORE_CFLAGS@ 

bin_PROGRAMS = e_phys_demo 

e_phys_demo_SOURCES = \
main.c \
test_snow.c \
demo.h

e_phys_demo_LDADD = $(top_builddir)/src/lib/libephys.la @EVAS_LIBS@ @ECORE_LIBS@
e_phys_demo_DEPENDENCIES = $(top_builddir)/src/lib/libephys.la

EXTRA_DIST = flake.png
