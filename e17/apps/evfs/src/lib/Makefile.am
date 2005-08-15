lib_LTLIBRARIES = libevfs.la

INCLUDES = @ecore_cflags@ @edb_cflags@ @xml2_cflags@ $(DEBUGFLAGS) -I. -I$(top_srcdir) -I$(top_srcdir)/src/include

libevfs_la_SOURCES   =                          \
        ../evfs.h                                  \
	libevfs.c				\
	evfs_commands.c				\
	../common/evfs_io.c			\
	../common/evfs_cleanup.c

libevfs_la_CFLAGS = -DLIB $(AM_CFLAGS)
	


