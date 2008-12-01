MAINTAINERCLEANFILES = Makefile.in
INCLUDES = -I$(top_srcdir) \
	   -I$(top_srcdir)/src/bin \
	   @EVAS_CFLAGS@ @ECORE_CFLAGS@ @EDJE_CFLAGS@

bin_PROGRAMS = exquisite exquisite-write

exquisite_SOURCES = \
main.h \
main.c \
theme.c \
ipc.c \
ipc.h

exquisite_LDADD = @EVAS_LIBS@ @ECORE_LIBS@ @EDJE_LIBS@

exquisite_write_SOURCES = \
write.c

exquisite_write_LDADD =
