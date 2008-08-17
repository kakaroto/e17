AUTOMAKE_OPTIONS     = 1.7 foreign

bin_PROGRAMS = evfs evfsworker evfscat evfscopy

MAINTAINERCLEANFILES = Makefile.in Makefile

DEBUGFLAGS = -DDEBUG -DDEBUG_NEST -W -Wall

INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/src/include @EET_CFLAGS@ @EVAS_CFLAGS@ @ECORE_CFLAGS@ @XML2_CFLAGS@ @EFREET_CFLAGS@ $(DEBUGFLAGS) -I.  -DBINDIR=\""$(bindir)"\"

evfs_SOURCES = \
	evfs_main.c \
	$(top_srcdir)/src/common/evfs_new.c \
	$(top_srcdir)/src/common/evfs_io.c \
	$(top_srcdir)/src/common/evfs_event_helper.c \
	$(top_srcdir)/src/common/evfs_common.c \
	$(top_srcdir)/src/common/evfs_filereference.c \
	evfs_server_handle.c \
	evfs_operation.c \
	evfs_operation_tasks.c \
	evfs_metadata.c \
	evfs_metadata_db.c \
	#evfs_mounts.c \
	$(top_srcdir)/src/common/evfs_vfolder.c \
	$(top_srcdir)/src/common/evfs_server.c \
	$(top_srcdir)/src/common/evfs_auth.c 

evfsworker_SOURCES = \
	evfs_worker.c \
        $(top_srcdir)/src/common/evfs_new.c \
        $(top_srcdir)/src/common/evfs_io.c \
        $(top_srcdir)/src/common/evfs_event_helper.c \
        $(top_srcdir)/src/common/evfs_common.c \
	$(top_srcdir)/src/common/evfs_filereference.c \
        evfs_server_handle.c \
        evfs_operation.c \
        evfs_operation_tasks.c \
        evfs_metadata.c \
        evfs_metadata_db.c \
        $(top_srcdir)/src/common/evfs_vfolder.c \
        $(top_srcdir)/src/common/evfs_server.c \
	$(top_srcdir)/src/common/evfs_auth.c


evfscat_SOURCES = \
	evfscat.c

evfscopy_SOURCES = \
        evfscopy.c


evfs_LDADD = $(top_builddir)/src/lib/libevfs.la @XML2_LIBS@ @SQLITE_LIBS@ @ECORE_LIBS@ @EVAS_LIBS@ @EET_LIBS@ @EFREET_LIBS@

evfscat_LDADD = $(top_builddir)/src/lib/libevfs.la @XML2_LIBS@ @ECORE_LIBS@ @EVAS_LIBS@ @EET_LIBS@ @EFREET_LIBS@

evfscopy_LDADD = $(top_builddir)/src/lib/libevfs.la @XML2_LIBS@ @ECORE_LIBS@ @EVAS_LIBS@ @EET_LIBS@ @EFREET_LIBS@

evfsworker_LDADD = $(top_builddir)/src/lib/libevfs.la @XML2_LIBS@ @ECORE_LIBS@ @EVAS_LIBS@ @EET_LIBS@ @EFREET_LIBS@


