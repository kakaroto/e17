AUTOMAKE_OPTIONS     = 1.4 foreign

MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/src/bin @EVAS_CFLAGS@ @ECORE_CFLAGS@ @EDJE_CFLAGS@

elementary_SOURCES = \
Elementary.h \
elm_priv.h \
elm_main.c \
elm_obj.c \
elm_callback.c \
elm_win.c

bin_PROGRAMS = alarm

alarm_SOURCES = \
main.c \
main.h \
$(elementary_SOURCES)

alarm_LDADD = @my_libs@ @EVAS_LIBS@ @ECORE_LIBS@ @EDJE_LIBS@
