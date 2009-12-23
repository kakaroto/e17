MAINTAINERCLEANFILES = Makefile.in
INCLUDES = -I$(top_srcdir) \
         -I$(top_srcdir)/src/bin \
         @ELM_QUICKPANEL_CFLAGS@

bin_PROGRAMS = elm_quickpanel

ELMQUICKPANELHEADERS = \
              elm_quickpanel.h

elm_quickpanel_SOURCES = \
            $(ELMQUICKPANELHEADERS) \
            main.c

elm_quickpanel_LDFLAGS = -export-dynamic
elm_quickpanel_LDADD = @ELM_QUICKPANEL_LIBS@
