MAINTAINERCLEANFILES = \
  Makefile.in aclocal.m4 config.guess \
  config.h.in config.sub configure install-sh \
  ltconfig ltmain.sh missing mkinstalldirs \
  stamp-h.in acconfig.h depcomp INSTALL *~

INCLUDES = \
  -I$(top_srcdir) \
  -I$(top_srcdir)/src/lib \
  -I$(top_srcdir)/src/bin \
  -DPACKAGE_LOCALE_DIR=\"$(datadir)/locale\" \
  -DUSE_EMOTE_CONFIG_H \
  @EMOTE_CFLAGS@

bin_PROGRAMS = \
  emote

EMOTEHEADERS = \
  em_includes.h \
  em_types.h \
  em_object.h \
  em_utils.h \
  em_config.h \
  em_gui.h \
  em_global.h

emote_SOURCES = \
  $(EMOTEHEADERS) \
  em_object.c \
  em_utils.c \
  em_config.c \
  em_gui.c \
  main.c

emote_LDADD = @EMOTE_LIBS@ @LTLIBINTL@ $(top_srcdir)/src/lib/libemote.la
emote_LDFLAGS = -export-dynamic
