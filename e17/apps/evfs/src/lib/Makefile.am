lib_LTLIBRARIES = libevfs.la

INCLUDES = @ecore_cflags@ @xml2_cflags@ $(DEBUGFLAGS) -I. -I$(top_srcdir) -I$(top_srcdir)/src/include

libevfs_la_SOURCES   =                          \
        ../include/evfs.h                       \
	../include/evfs_cleanup.h		\
	../include/evfs_commands.h		\
	../include/evfs_debug.h			\
	../include/evfs_event_helper.h		\
	../include/evfs_io.h			\
	../include/evfs_macros.h		\
	../include/evfs_new.h			\
	../include/evfs_plugin.h		\
	../include/evfs_server_handle.h		\
	libevfs.c				\
	evfs_commands.c				\
	../common/evfs_io.c			\
	../common/evfs_cleanup.c

libevfs_la_CFLAGS = -DLIB $(AM_CFLAGS)
