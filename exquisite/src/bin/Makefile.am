MAINTAINERCLEANFILES = Makefile.in

bin_PROGRAMS = exquisite exquisite-write

exquisite_SOURCES = \
main.h \
main.c \
theme.c \
ipc.c \
ipc.h

exquisite_CPPFLAGS = \
-I. \
-I$(top_srcdir)/src/lib \
-I$(top_builddir)/src/lib \
-DPACKAGE_BIN_DIR=\"$(bindir)\" \
-DPACKAGE_LIB_DIR=\"$(libdir)\" \
-DPACKAGE_DATA_DIR=\"$(pkgdatadir)\" \
@EXQUISITE_READ_CFLAGS@

exquisite_LDADD = $(top_builddir)/src/lib/libexquisite.la @EXQUISITE_READ_LIBS@

exquisite_write_SOURCES = write.c
exquisite_write_CPPFLAGS = \
-I. \
-I$(top_srcdir)/src/lib \
-I$(top_builddir)/src/lib \
@EXQUISITE_WRITE_CFLAGS@
exquisite_write_LDADD = @EXQUISITE_WRITE_LIBS@
