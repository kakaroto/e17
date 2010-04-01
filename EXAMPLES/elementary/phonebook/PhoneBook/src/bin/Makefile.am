MAINTAINERCLEANFILES = Makefile.in

INCLUDES = \
-I$(top_srcdir) \
-I$(top_srcdir)/src/bin \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
@ELEMENTARY_CFLAGS@

bin_PROGRAMS = Phone_Book_Sample

Phone_Book_Sample_LDADD = @ELEMENTARY_LIBS@
Phone_Book_Sample_SOURCES = main.c elm_widget_helper.c db.c

noinst_HEADERS = elm_widget_helper.h db.h
EXTRA_DIST = elm_widget_helper.h db.h
