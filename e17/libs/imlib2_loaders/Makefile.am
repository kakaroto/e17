## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = 1.4 foreign

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = Makefile.in imlib2_loaders.spec

SUBDIRS = src libltdl

CFLAGS_EXTRA = -I$(includedir) -I$(top_srcdir) -I$(top_srcdir)/src

EXTRA_DIST = \
acinclude.m4 \
imlib2_loaders.spec.in \
imlib2_loaders.spec

