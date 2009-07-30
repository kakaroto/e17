MAINTAINERCLEANFILES = Makefile.in

if BUILD_SCRIPT_NEKO

AM_CPPFLAGS = \
-I. \
-I$(top_srcdir)/src/include \
@ETK_CFLAGS@

noinst_LTLIBRARIES = libeon_script_neko.la

libeon_script_neko_la_SOURCES = \
neko_main.c

libeon_script_neko_la_LIBADD = \
@NEKO_LIBS@

endif
