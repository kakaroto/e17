MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I$(top_srcdir)/src `gtk-config --cflags`

bin_PROGRAMS = edb_gtk_ed

edb_gtk_ed_DEPENDENCIES = $(top_builddir)/src/libedb.la

edb_gtk_ed_SOURCES = \
	main.c \
	callbacks.h \
	callbacks.c \
	support.h \
	support.c \
	interface.h \
	interface.c

edb_gtk_ed_LDFLAGS = `gtk-config --libs`
edb_gtk_ed_LDADD = $(top_builddir)/src/libedb.la
