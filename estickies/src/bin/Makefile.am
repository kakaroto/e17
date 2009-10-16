AUTOMAKE_OPTIONS     = 1.4 foreign

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
theme_chooser.h \
about.h \
plugin.h

estickies_SOURCES = \
stickies.c \
conf.c \
theme_chooser.c \
about.c \
plugin.c \
$(estickies_INCLUDES)
estickies_LDADD = @ESTICKIES_LIBS@
