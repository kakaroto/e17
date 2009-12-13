MAINTAINERCLEANFILES = Makefile.in
INCLUDES = -I$(top_srcdir) \
         -I$(top_srcdir)/src/bin \
         @ELM_FULLSCREEN_CFLAGS@

bin_PROGRAMS = elm_fullscreen

ELMFULLSCREENHEADERS = \
              elm_fullscreen.h

elm_fullscreen_SOURCES = \
            $(ELMFULLSCREENHEADERS) \
            main.c

elm_fullscreen_LDFLAGS = -export-dynamic
elm_fullscreen_LDADD = @ELM_FULLSCREEN_LIBS@
