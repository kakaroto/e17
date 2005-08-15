bin_PROGRAMS = evfs

MAINTAINERCLEANFILES = Makefile.in Makefile

DEBUGFLAGS = -DDEBUG -DDEBUG_NEST -W -Wall

INCLUDES = @ecore_cflags@ @edb_cflags@ @xml2_cflags@ $(DEBUGFLAGS) -I. -I$(top_srcdir) -I$(top_srcdir)/src/include

include_HEADERS = \
	../include/evfs.h

evfs_SOURCES = \
	evfs_main.c \
	evfs_cleanup.c \
	../common/evfs_new.c \
	../common/evfs_io.c

evfs_LDADD = -L$(top_builddir)/src/lib -levfs @edb_libs@ @ecore_libs@ @xml2_libs@ @fam_libs@


