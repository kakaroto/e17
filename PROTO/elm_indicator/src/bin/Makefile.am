MAINTAINERCLEANFILES = Makefile.in
INCLUDES = -I$(top_srcdir) \
         -I$(top_srcdir)/src/bin \
         @ELM_INDICATOR_CFLAGS@

bin_PROGRAMS = elm_indicator

ELMINDICATORHEADERS = \
              elm_indicator.h \
              elm_home.h \
              elm_ind_win.h

elm_indicator_SOURCES = \
            $(ELMINDICATORHEADERS) \
            main.c \
            elm_home.c \
            elm_ind_win.c

elm_indicator_LDFLAGS = -export-dynamic
elm_indicator_LDADD = @ELM_INDICATOR_LIBS@
