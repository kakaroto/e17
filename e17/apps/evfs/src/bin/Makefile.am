bin_PROGRAMS = evfs

MAINTAINERCLEANFILES = Makefile.in Makefile

DEBUGFLAGS = -DDEBUG -DDEBUG_NEST -W -Wall

INCLUDES = @ecore_cflags@ @xml2_cflags@ $(DEBUGFLAGS) -I. -I$(top_srcdir) -I$(top_srcdir)/src/include

include_HEADERS = \
	../include/evfs.h

evfs_SOURCES = \
	evfs_main.c \
	evfs_cleanup.c \
	$(top_srcdir)/src/common/evfs_new.c \
	$(top_srcdir)/src/common/evfs_io.c \
	$(top_srcdir)/src/common/evfs_event_helper.c \
	$(top_srcdir)/src/common/evfs_common.c \
	evfs_server_handle.c

evfs_LDADD = -L$(top_builddir)/src/lib -levfs @ecore_libs@ @xml2_libs@


