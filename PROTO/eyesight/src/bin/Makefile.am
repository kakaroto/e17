
MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir) \
-I$(top_srcdir)/src/lib \
-I$(top_srcdir)/src/modules/img \
-I$(top_srcdir)/src/modules/pdf \
-I$(top_srcdir)/src/modules/ps \
-I$(top_srcdir)/src/modules/txt \
-DPACKAGE_BIN_DIR=\"$(bindir)\" \
-DPACKAGE_DATA_DIR=\"$(pkgdatadir)\" \
@EYESIGHT_CFLAGS@ \
@EYESIGHT_BIN_CFLAGS@

bin_PROGRAMS =

if BUILD_EYESIGHT

bin_PROGRAMS += envision

endif

envision_SOURCES = \
envision_config.c \
envision_load.c \
envision_gui.c \
envision_input.c \
envision_main.c \
envision_win.c \
envision.h \
envision_config.h \
envision_gui.h \
envision_input.h \
envision_load.h \
envision_win.h
#eyesight.c eyesight_popup.c eyesight_popup.h

envision_LDADD = @EYESIGHT_BIN_LIBS@ $(top_builddir)/src/lib/libeyesight.la

filesdir = $(datadir)/$(PACKAGE)
files_DATA = gradient.png

EXTRA_DIST = gradient.png
