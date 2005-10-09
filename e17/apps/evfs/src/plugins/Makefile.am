## Process this file with automake to produce Makefile.in
AUTOMAKE_OPTIONS     = 1.7 foreign

LDFLAGS              = -rdynamic -shared
INCLUDES             = -I. -I$(top_srcdir) -I$(top_srcdir)/src/include

pkgdir =  $(libdir)/evfs/plugins/file

POSIX_L = posix.la

SAMBA_L = samba.la

pkg_LTLIBRARIES         = $(POSIX_L) $(SAMBA_L)

posix_la_SOURCES = evfs_fs_posix.c \
			../common/evfs_debug.c \
			../common/evfs_event_helper.c

samba_la_SOURCES = evfs_fs_samba.c \
                        ../common/evfs_debug.c \
                        ../common/evfs_event_helper.c

	
posix_la_CFLAGS =  @ecore_cflags@
posix_la_LDFLAGS = -no-undefined -module -avoid-version
posix_la_LIBADD =  @ecore_libs@

samba_la_CFLAGS =  @ecore_cflags@
samba_la_LDFLAGS = -no-undefined -module -avoid-version -lsmbclient
samba_la_LIBADD =  @ecore_libs@
