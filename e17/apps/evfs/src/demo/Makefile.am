INCLUDES = -I$(top_srcdir)/src/include

bin_PROGRAMS = evfsdemo

evfsdemo_SOURCES = \
	evfs_demo.c

evfsdemo_LDADD = $(top_builddir)/src/lib/libevfs.la @ecore_libs@
evfsdemo_CFLAGS = @ecore_cflags@
