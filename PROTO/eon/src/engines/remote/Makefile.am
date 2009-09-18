MAINTAINERCLEANFILES = Makefile.in

if BUILD_ENGINE_IPC

AM_CPPFLAGS = \
-I. \
-I$(top_srcdir)/src/include \
@ECORE_IPC_CFLAGS@ @EON_CFLAGS@

noinst_LTLIBRARIES = libeon_engine_remote.la

libeon_engine_remote_la_SOURCES = \
remote_engine.c

libeon_engine_remote_la_LIBADD = \
@ECORE_IPC_LIBS@

endif
