## $Id$

SUBDIRS = plugins

AM_CFLAGS = $(ECORE_CFLAGS)  \
            $(EVAS_CFLAGS)   \
            $(EDJE_CFLAGS)   \
            $(ESMART_CFLAGS)

bin_PROGRAMS = embrace

embrace_SOURCES = main.c                            \
                  embrace.c embrace.h               \
                  embrace_plugin.c embrace_plugin.h \
                  mailbox.c mailbox.h

embrace_LDADD = $(ECORE_LIBS)  \
                $(EVAS_LIBS)   \
                $(EDJE_LIBS)   \
                $(ESMART_LIBS) \
                $(DL_LIBS)

embrace_LDFLAGS = -export-dynamic
