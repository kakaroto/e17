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

pkg_LTLIBRARIES         = $(POSIX_L) $(SAMBA_L)

posix_la_SOURCES = evfs_fs_posix.c \
			$(top_srcdir)/src/common/evfs_debug.c \
			$(top_srcdir)/src/common/evfs_event_helper.c

samba_la_SOURCES = evfs_fs_samba.c \
                        $(top_srcdir)/src/common/evfs_debug.c \
                        $(top_srcdir)/src/common/evfs_event_helper.c

	
posix_la_CFLAGS =  @ecore_cflags@
posix_la_LDFLAGS = -module -avoid-version
posix_la_LIBADD =  @ecore_libs@ -L$(top_builddir)/src/lib -levfs

samba_la_CFLAGS =  @ecore_cflags@
samba_la_LDFLAGS = -module -avoid-version -lsmbclient
samba_la_LIBADD =  @ecore_libs@ -L$(top_builddir)/src/lib -levfs
