MAINTAINERCLEANFILES = Makefile.in

if EDB_VT_ED
edb_vt_ed_PROGRAM = edb_vt_ed
endif
if EDB_GTK_ED
edb_gtk_ed_DIR = edb_gtk_ed
endif

SUBDIRS =$(edb_gtk_ed_DIR)

INCLUDES = -I$(top_srcdir)/src

bin_PROGRAMS = edb_ed $(edb_vt_ed_PROGRAM)

edb_ed_DEPENDENCIES = $(top_builddir)/src/libedb.la
edb_ed_SOURCES = edb_ed.c
edb_ed_LDADD = $(top_builddir)/src/libedb.la

edb_vt_ed_DEPENDENCIES = $(top_builddir)/src/libedb.la
edb_vt_ed_SOURCES = edb_vt_ed.c
edb_vt_ed_LDADD = -lncurses $(top_builddir)/src/libedb.la
