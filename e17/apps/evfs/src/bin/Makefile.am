AUTOMAKE_OPTIONS     = 1.7 foreign

bin_PROGRAMS = evfs evfscat

MAINTAINERCLEANFILES = Makefile.in Makefile

DEBUGFLAGS = -DDEBUG -DDEBUG_NEST -W -Wall

INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/src/include @eet_cflags@ @ecore_cflags@ @xml2_cflags@ $(DEBUGFLAGS) -I.

evfs_SOURCES = \
	evfs_main.c \
	evfs_cleanup.c \
	$(top_srcdir)/src/common/evfs_new.c \
	$(top_srcdir)/src/common/evfs_io.c \
	$(top_srcdir)/src/common/evfs_event_helper.c \
	$(top_srcdir)/src/common/evfs_common.c \
	evfs_server_handle.c \ 
	evfs_operation.c \
	$(top_srcdir)/src/common/evfs_vfolder.c

evfscat_SOURCES = \
	evfscat.c

evfs_LDADD = $(top_builddir)/src/lib/libevfs.la @ecore_libs@ @xml2_libs@ @eet_libs@

evfscat_LDADD = $(top_builddir)/src/lib/libevfs.la @ecore_libs@ @xml2_libs@ @eet_libs@


