MAINTAINERCLEANFILES = Makefile.in config.h.in

SUBDIRS = protocols

INCLUDES = \
  -I$(top_srcdir) \
  -I$(top_srcdir)/src/lib \
  -I$(top_builddir)/src/lib \
  -DPACKAGE_LOCALE_DIR=\"$(datadir)/locale\" \
  -DUSE_EMOTE_CONFIG_H \
  @LIBEMOTE_CFLAGS@

AM_CPPFLAGS=-DPACKAGE_LIB_DIR=\"$(libdir)\"

pkgdir = $(libdir)
pkg_LTLIBRARIES = libemote.la

libemote_la_SOURCES = libemote.h Emote.h emote_object.c \
                      emote_main.c emote_protocol.c
libemote_la_LDFLAGS = -version-info 0:1:0
libemote_la_LIBADD = @dlopen_libs@ @LIBEMOTE_LIBS@

installed_mainheaderdir = $(includedir)/emote
dist_installed_mainheader_DATA = Emote.h
