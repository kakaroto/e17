
MAINTAINERCLEANFILES = Makefile.in

AM_CFLAGS = \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
-Wall -g -I$(top_srcdir)/src/photo_manager @EPHOTO_MANAGER_CFLAGS@

bin_PROGRAMS = ephoto_manager_db_sync

ephoto_manager_db_sync_SOURCES = bin_sync.c
ephoto_manager_db_sync_LDADD = $(top_builddir)/src/photo_manager/libephoto_manager.la @EPHOTO_MANAGER_LIBS@

