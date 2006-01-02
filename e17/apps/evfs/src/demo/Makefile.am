INCLUDES = -I$(top_srcdir)/src/include

bin_PROGRAMS = evfsdemo evfsthumb

evfsdemo_SOURCES = \
	evfs_demo.c

evfsthumb_SOURCES = \
        evfs_thumb.c
	


evfsdemo_LDADD = $(top_builddir)/src/lib/libevfs.la @ecore_libs@
evfsdemo_CFLAGS = @ecore_cflags@

evfsthumb_LDADD = $(top_builddir)/src/lib/libevfs.la @ecore_libs@ -levas -ledje -lImlib2 -lpng
evfsthumb_CFLAGS = @ecore_cflags@

