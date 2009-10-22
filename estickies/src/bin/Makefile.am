MAINTAINERCLEANFILES = Makefile.in

INCLUDES = \
-Wall -g \
-I$(top_srcdir) \
-I$(top_srcdir)/src/bin \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
@ESTICKIES_CFLAGS@

bin_PROGRAMS = estickies

estickies_INCLUDES = stickies.h

estickies_SOURCES = \
sticky_gui.c \
sticky_manipulation.c \
sticky_window.c \
stickies.c \
conf.c \
config_gui.c \
about.c \
plugin.c \
$(estickies_INCLUDES)
estickies_LDADD = @ESTICKIES_LIBS@
