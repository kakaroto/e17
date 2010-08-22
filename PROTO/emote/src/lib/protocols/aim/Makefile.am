MAINTAINERCLEANFILES = Makefile.in config.h.in

PROTOCOL = aim

INCLUDES = \
  -I$(top_srcdir) \
  -I$(top_srcdir)/src/lib/protocols/$(PROTOCOL) \
  -I$(top_srcdir)/src/lib \
  -DPACKAGE_LOCALE_DIR=\"$(datadir)/locale\" \
  -DPACKAGE_LIB_DIR=\"$(libdir)/emote/protocols\" \
  -DUSE_EMOTE_CONFIG_H \
  @AIM_CFLAGS@

protocoldir = $(libdir)/emote/protocols

aimdir = $(protocoldir)
aim_LTLIBRARIES = aim.la
aim_la_SOURCES = \
  aim.h \
  aim.c

aim_la_LIBADD = @AIM_CFLAGS@
aim_la_LDFLAGS = -module -avoid-version
aim_la_LIBTOOLFLAGS = --tag=disable-static
