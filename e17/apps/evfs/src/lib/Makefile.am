AUTOMAKE_OPTIONS     = 1.7 foreign

lib_LTLIBRARIES = libevfs.la

INCLUDES = @eet_cflags@ @ecore_cflags@ @xml2_cflags@ $(DEBUGFLAGS) -I. -I$(top_srcdir) -I$(top_srcdir)/src/include \
	   -DBINDIR=\""$(bindir)"\"

libevfs_la_SOURCES   =                          \
        $(top_srcdir)/src/include/evfs.h                       \
	$(top_srcdir)/src/include/evfs_cleanup.h		\
	$(top_srcdir)/src/include/evfs_commands.h		\
	$(top_srcdir)/src/include/evfs_debug.h			\
	$(top_srcdir)/src/include/evfs_event_helper.h		\
	$(top_srcdir)/src/include/evfs_io.h			\
	$(top_srcdir)/src/include/evfs_macros.h		\
	$(top_srcdir)/src/include/evfs_new.h			\
	$(top_srcdir)/src/include/evfs_plugin.h		\
	$(top_srcdir)/src/include/evfs_server_handle.h		\
	libevfs.c				\
	evfs_commands.c				\
	$(top_srcdir)/src/common/evfs_io.c			\
	$(top_srcdir)/src/common/evfs_cleanup.c

libevfs_la_LIBADD = \
	@ecore_libs@ \
	@eet_libs@

libevfs_la_CFLAGS = -DLIB $(AM_CFLAGS)
