MAINTAINERCLEANFILES = \
  Makefile.in aclocal.m4 config.guess \
  config.h.in config.sub configure install-sh \
  ltconfig ltmain.sh missing mkinstalldirs \
  stamp-h.in acconfig.h depcomp INSTALL *~

INCLUDES = \
  -I$(top_srcdir) \
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
  em_irc.h \
  em_irc_parse.h \
  emote.h

emote_SOURCES = \
  $(EMOTEHEADERS) \
  em_object.c \
  em_utils.c \
  em_config.c \
  em_gui.c \
  em_irc.c \
  em_irc_parse.c \
  main.c

emote_LDADD = @EMOTE_LIBS@ @LTLIBINTL@
emote_LDFLAGS = -export-dynamic
