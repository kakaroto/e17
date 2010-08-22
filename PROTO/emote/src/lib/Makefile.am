## Process this file with automake to produce Makefile.in

SUBDIRS = protocols

INCLUDES = \
  -I$(top_srcdir) \
  -I$(top_srcdir)/src/include \
  -I$(top_srcdir)/src/lib \
  -I$(top_builddir)/src/lib \
  @EMOTE_CFLAGS@

AM_CPPFLAGS=-DPACKAGE_LIB_DIR=\"$(libdir)\"

pkgdir = $(libdir)

pkg_LTLIBRARIES = libemote.la

libemote_la_SOURCES = em_main.c em_protocol.c
libemote_la_LDFLAGS = -version-info 0:1:0
libemote_la_LIBADD = @dlopen_libs@ @EMOTE_LIBS@

MAINTAINERCLEANFILES = Makefile.in config.h.in
