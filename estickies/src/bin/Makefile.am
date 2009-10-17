MAINTAINERCLEANFILES = Makefile.in

INCLUDES = \
-Wall -g \
-I$(top_srcdir) \
-I$(top_srcdir)/src/bin \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
@ESTICKIES_CFLAGS@

bin_PROGRAMS = estickies

estickies_INCLUDES = \
stickies.h \
conf.h \
config_gui.h \
about.h \
plugin.h

estickies_SOURCES = \
stickies.c \
conf.c \
config_gui.c \
about.c \
plugin.c \
$(estickies_INCLUDES)
estickies_LDADD = @ESTICKIES_LIBS@
