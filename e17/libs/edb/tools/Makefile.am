INCLUDES = -I$(top_srcdir)/src -I/usr/local/include `gtk-config --cflags`


if EDB_VT_ED
edb_vt_ed_PROGRAM = edb_vt_ed
endif
if EDB_GTK_ED
edb_gtk_ed_PROGRAM = edb_gtk_ed
endif

bin_PROGRAMS = edb_ed $(edb_vt_ed_PROGRAM) $(edb_gtk_ed_PROGRAM)

edb_ed_DEPENDENCIES = $(top_builddir)/src/libedb.la
edb_ed_SOURCES = edb_ed.c
edb_ed_LDFLAGS =
edb_ed_LDADD = $(top_builddir)/src/libedb.la

edb_vt_ed_DEPENDENCIES = $(top_builddir)/src/libedb.la
edb_vt_ed_SOURCES = edb_vt_ed.c
edb_vt_ed_LDFLAGS = -lncurses
edb_vt_ed_LDADD = $(top_builddir)/src/libedb.la

edb_gtk_ed_DEPENDENCIES = $(top_builddir)/src/libedb.la
edb_gtk_ed_SOURCES = edb_gtk_ed.c
edb_gtk_ed_LDFLAGS = `gtk-config --libs`
edb_gtk_ed_LDADD = $(top_builddir)/src/libedb.la
