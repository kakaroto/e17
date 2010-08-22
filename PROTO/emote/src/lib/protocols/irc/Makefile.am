## Process this file with automake to produce Makefile.in

INCLUDES = \
  -I$(top_srcdir) \
  -I$(top_srcdir)/src/include \
  -I$(top_srcdir)/src/lib \
  -I$(top_builddir)/src/lib \
  @EMOTE_CFLAGS@

pkgdir = $(libdir)/emote/protocols

pkg_LTLIBRARIES = irc.la

irc_la_SOURCES = irc_main.c
irc_la_LDFLAGS = -module -avoid-version @EMOTE_LIBS@

MAINTAINERCLEANFILES = Makefile.in config.h.in
