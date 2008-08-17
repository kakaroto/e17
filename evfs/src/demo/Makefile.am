INCLUDES = -I$(top_srcdir)/src/include

bin_PROGRAMS = evfsdemo

evfsdemo_SOURCES = \
	evfs_demo.c

evfsdemo_LDADD = $(top_builddir)/src/lib/libevfs.la @ECORE_LIBS@ @EVAS_LIBS@
evfsdemo_CFLAGS = @ECORE_CFLAGS@ @EFREET_CFLAGS@
