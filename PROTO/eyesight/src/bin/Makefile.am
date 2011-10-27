
MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir) \
-I$(top_srcdir)/src/lib \
-I$(top_srcdir)/src/modules/img \
-I$(top_srcdir)/src/modules/pdf \
-I$(top_srcdir)/src/modules/ps \
-I$(top_srcdir)/src/modules/txt \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
@EYESIGHT_CFLAGS@ \
@EYESIGHT_BIN_CFLAGS@

bin_PROGRAMS =

if BUILD_EYESIGHT

bin_PROGRAMS += eyesight

endif

eyesight_SOURCES = eyesight.c eyesight_popup.c eyesight_popup.h

eyesight_LDADD = @EYESIGHT_BIN_LIBS@ $(top_builddir)/src/lib/libeyesight.la

filesdir = $(datadir)/$(PACKAGE)
files_DATA = gradient.png

#EXTRA_DIST = $(files_DATA)
