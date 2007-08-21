AUTOMAKE_OPTIONS     = 1.7 foreign

lib_LTLIBRARIES = libevfs.la

INCLUDES = -I$(top_srcdir)/src/include @EET_CFLAGS@ @ECORE_CFLAGS@ @EFREET_CFLAGS@ $(DEBUGFLAGS) -I. -I$(top_srcdir) \
	   -DBINDIR=\""$(bindir)"\"

libevfs_la_SOURCES   = \
        $(top_srcdir)/src/include/evfs.h \
	$(top_srcdir)/src/include/evfs_cleanup.h \
	$(top_srcdir)/src/include/evfs_client.h \
	$(top_srcdir)/src/include/evfs_command.h \
	$(top_srcdir)/src/include/evfs_commands.h \
	$(top_srcdir)/src/include/evfs_common.h \
	$(top_srcdir)/src/include/evfs_debug.h \
	$(top_srcdir)/src/include/evfs_event.h \
	$(top_srcdir)/src/include/evfs_event_helper.h \
	$(top_srcdir)/src/include/evfs_filereference.h \
	$(top_srcdir)/src/include/evfs_io.h \
	$(top_srcdir)/src/include/evfs_macros.h \
	$(top_srcdir)/src/include/evfs_misc.h \
	$(top_srcdir)/src/include/evfs_new.h \
	$(top_srcdir)/src/include/evfs_operation.h \
	$(top_srcdir)/src/include/evfs_operation_tasks.h \
	$(top_srcdir)/src/include/evfs_plugin.h \
	$(top_srcdir)/src/include/evfs_server.h \
	$(top_srcdir)/src/include/evfs_server_handle.h \
	$(top_srcdir)/src/include/evfs_vfolder.h \
	$(top_srcdir)/src/include/tarpet.h \
	libevfs.c \
	evfs_commands.c \
	$(top_srcdir)/src/common/evfs_io.c \
	$(top_srcdir)/src/common/evfs_cleanup.c \
	$(top_srcdir)/src/common/evfs_vfolder.c \
	$(top_srcdir)/src/common/evfs_misc.c \
	$(top_srcdir)/src/common/evfs_common.c \
	$(top_srcdir)/src/common/evfs_filereference.c \
	$(top_srcdir)/src/common/evfs_command.c \
	$(top_srcdir)/src/bin/evfs_metadata.c \
	$(top_srcdir)/src/bin/evfs_metadata_db.c \
	$(top_srcdir)/src/common/evfs_server.c \
	$(top_srcdir)/src/bin/evfs_trash.c 

libevfs_la_LIBADD = \
	@ECORE_LIBS@ \
	@EET_LIBS@ \
	@SQLITE_LIBS@ \
	@EFREET_LIBS@ 

libevfs_la_CFLAGS = -DLIB $(AM_CFLAGS)
