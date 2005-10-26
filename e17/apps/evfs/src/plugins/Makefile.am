## Process this file with automake to produce Makefile.in
AUTOMAKE_OPTIONS     = 1.7 foreign

LDFLAGS              = -rdynamic -shared
INCLUDES             = -I. -I$(top_srcdir) -I$(top_srcdir)/src/include

pkgdir =  $(libdir)/evfs/plugins/file

POSIX_L = posix.la

if HAVE_SAMBA
SAMBA_L = samba.la
else
SAMBA_L =
endif

TAR_L = tar.la

pkg_LTLIBRARIES         = $(POSIX_L) $(SAMBA_L) $(TAR_L)

posix_la_SOURCES = evfs_fs_posix.c \
			$(top_srcdir)/src/common/evfs_debug.c \
			$(top_srcdir)/src/common/evfs_event_helper.c
			

tar_la_SOURCES = evfs_fs_tar.c \
                        $(top_srcdir)/src/common/evfs_debug.c \
                        $(top_srcdir)/src/common/evfs_event_helper.c \
			$(top_srcdir)/src/bin/evfs_server_handle.c \
			$(top_srcdir)/src/common/evfs_common.c 


samba_la_SOURCES = evfs_fs_samba.c \
                        $(top_srcdir)/src/common/evfs_debug.c \
                        $(top_srcdir)/src/common/evfs_event_helper.c

	
posix_la_CFLAGS =  @ecore_cflags@
posix_la_LDFLAGS = -module -avoid-version
posix_la_LIBADD =  @ecore_libs@ -L$(top_builddir)/src/lib -levfs

samba_la_CFLAGS =  @ecore_cflags@
samba_la_LDFLAGS = -module -avoid-version -lsmbclient
samba_la_LIBADD =  @ecore_libs@ -L$(top_builddir)/src/lib -levfs

tar_la_CFLAGS =  @ecore_cflags@
tar_la_LDFLAGS = -module -avoid-version
tar_la_LIBADD =  @ecore_libs@ -L$(top_builddir)/src/lib -levfs

