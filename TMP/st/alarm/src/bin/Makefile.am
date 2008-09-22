AUTOMAKE_OPTIONS     = 1.4 foreign

MAINTAINERCLEANFILES = Makefile.in

INCLUDES = \
-I$(top_srcdir) \
-I$(top_srcdir)/src/bin \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
@COMMON_CFLAGS@ \
@ALARM_CFLAGS@

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

bin_PROGRAMS = \
alarm \
funeral \
elementary_test

###---### Alarm clock main program - uses funeral
alarm_SOURCES = $(elementary_SOURCES) \
main.c
alarm_LDADD = @my_libs@ @COMMON_LIBS@ @ALARM_LIBS@

###---### A time scheduler (like at) but talks dbus, uses rtc and has seconds
funeral_SOURCES = \
funeral_main.c
funeral_LDADD = @my_libs@ @COMMON_LIBS@

###---### Just a widget test app
elementary_test_SOURCES = $(elementary_SOURCES) \
test.c
elementary_test_LDADD = @my_libs@ @COMMON_LIBS@ @ALARM_LIBS@

