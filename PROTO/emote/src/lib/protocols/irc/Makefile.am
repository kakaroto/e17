MAINTAINERCLEANFILES = Makefile.in config.h.in

PROTOCOL = irc

INCLUDES = \
  -I$(top_srcdir) \
  -I$(top_srcdir)/src/lib/protocols/$(PROTOCOL) \
  -I$(top_srcdir)/src/lib \
  -DPACKAGE_LOCALE_DIR=\"$(datadir)/locale\" \
  -DPACKAGE_LIB_DIR=\"$(libdir)/emote/protocols\" \
  -DUSE_EMOTE_CONFIG_H \
  @IRC_CFLAGS@

protocoldir = $(libdir)/emote/protocols

ircdir = $(protocoldir)
irc_LTLIBRARIES = irc.la
irc_la_SOURCES = \
  irc.h \
  irc.c

irc_la_LIBADD = @IRC_CFLAGS@
irc_la_LDFLAGS = -module -avoid-version
irc_la_LIBTOOLFLAGS = --tag=disable-static
