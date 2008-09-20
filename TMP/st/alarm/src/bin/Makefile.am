AUTOMAKE_OPTIONS     = 1.4 foreign

MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/src/bin @EVAS_CFLAGS@ @ECORE_CFLAGS@ @EDJE_CFLAGS@

elementary_SOURCES = \
Elementary.h \
elm_priv.h \
elm_main.c \
elm_obj.c \
elm_callback.c \
elm_win.c \
elm_theme.c \
elm_widget.c \
elm_bg.c \
elm_scroller.c \
elm_label.c \
elm_box.c \
elm_table.c \
elm_button.c \
elm_icon.c \
elm_toggle.c \
elm_clock.c \
elm_frame.c \
elm_pad.c \
\
els_pan.c \
els_pan.h \
els_scroller.c \
els_scroller.h \
els_box.c \
els_box.h \
els_table.c \
els_table.h \
els_icon.c \
els_icon.h

bin_PROGRAMS = alarm alarm_test

alarm_test_SOURCES = \
test.c \
$(elementary_SOURCES)

alarm_test_LDADD = @my_libs@ @EVAS_LIBS@ @ECORE_LIBS@ @EDJE_LIBS@

alarm_SOURCES = \
main.c \
$(elementary_SOURCES)

alarm_LDADD = @my_libs@ @EVAS_LIBS@ @ECORE_LIBS@ @EDJE_LIBS@
