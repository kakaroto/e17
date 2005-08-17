lib_LTLIBRARIES = libevfs.la

INCLUDES = @ecore_cflags@ @xml2_cflags@ $(DEBUGFLAGS) -I. -I$(top_srcdir) -I$(top_srcdir)/src/include

libevfs_la_SOURCES   =                          \
        ../evfs.h                                  \
	../evfs_cleanup.h			\
	../evfs_commands.h			\
	../evfs_debug.h				\
	../evfs_event_helper.h			\
	../evfs_io.h				\
	../evfs_macros.h			\
	../evfs_new.h				\
	../evfs_plugin.h			\
	../evfs_server_handle.h			\
	libevfs.c				\
	evfs_commands.c				\
	../common/evfs_io.c			\
	../common/evfs_cleanup.c

libevfs_la_CFLAGS = -DLIB $(AM_CFLAGS)
	


