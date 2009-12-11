MAINTAINERCLEANFILES = Makefile.in
INCLUDES = -I$(top_srcdir) \
         -I$(top_srcdir)/src/bin \
         -I$(includedir) \
         @ELM_SOFTKEY_CFLAGS@

bin_PROGRAMS = elm_softkey

ELMSOFTKEYHEADERS = \
              elm_softkey.h

elm_softkey_SOURCES = \
            $(ELMSOFTKEYHEADERS) \
            main.c

elm_softkey_LDFLAGS = -export-dynamic
elm_softkey_LDADD = @ELM_SOFTKEY_LIBS@
