INCLUDES = -I$(top_srcdir)/src/include

bin_PROGRAMS = evfsdemo

evfsdemo_SOURCES = \
	evfs_demo.c


evfsdemo_LDADD = -L$(top_builddir)/src/lib -levfs @ecore_libs@
evfsdemo_CFLAGS = @ecore_cflags@
