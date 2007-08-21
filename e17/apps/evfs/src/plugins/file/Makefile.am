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

if HAVE_CURL
FTP_L = ftp.la
else
FTP_L =
endif

SFTP_L = sftp.la

TAR_L = tar.la

BZIP2_L = bzip2.la

GZIP_L = gzip.la

VFOLDER_L = vfolder.la

TRASH_L = trash.la


pkg_LTLIBRARIES         = $(POSIX_L) $(SAMBA_L) $(TAR_L) $(BZIP2_L) $(FTP_L) $(GZIP_L) $(SFTP_L) $(VFOLDER_L) $(TRASH_L)

trash_la_SOURCES = evfs_fs_trash.c \
                        $(top_srcdir)/src/common/evfs_debug.c \
                        $(top_srcdir)/src/common/evfs_event_helper.c \
                        $(top_srcdir)/src/common/evfs_misc.c


posix_la_SOURCES = evfs_fs_posix.c \
			$(top_srcdir)/src/common/evfs_debug.c \
			$(top_srcdir)/src/common/evfs_event_helper.c \
			$(top_srcdir)/src/common/evfs_misc.c \
			$(top_srcdir)/src/common/evfs_command.c \
			$(top_srcdir)/src/common/evfs_filereference.c

vfolder_la_SOURCES = evfs_fs_vfolder.c \
                        $(top_srcdir)/src/common/evfs_debug.c \
                        $(top_srcdir)/src/common/evfs_event_helper.c \
                        $(top_srcdir)/src/common/evfs_misc.c \
                        $(top_srcdir)/src/common/evfs_command.c \
                        $(top_srcdir)/src/common/evfs_filereference.c

tar_la_SOURCES = evfs_fs_tar.c \
                        $(top_srcdir)/src/common/evfs_debug.c \
                        $(top_srcdir)/src/common/evfs_event_helper.c \
			$(top_srcdir)/src/bin/evfs_server_handle.c \
			$(top_srcdir)/src/common/evfs_common.c \
                        $(top_srcdir)/src/common/evfs_command.c \
                        $(top_srcdir)/src/common/evfs_filereference.c

sftp_la_SOURCES = evfs_fs_sftp.c \
                        $(top_srcdir)/src/plugins/file/sftp.h \
                        $(top_srcdir)/src/common/evfs_debug.c \
                        $(top_srcdir)/src/common/evfs_event_helper.c \
                        $(top_srcdir)/src/bin/evfs_server_handle.c \
                        $(top_srcdir)/src/common/evfs_common.c \
                        $(top_srcdir)/src/common/evfs_command.c \
                        $(top_srcdir)/src/common/evfs_filereference.c											

bzip2_la_SOURCES = evfs_fs_bzip2.c \
                        $(top_srcdir)/src/common/evfs_debug.c \
                        $(top_srcdir)/src/common/evfs_event_helper.c \
                        $(top_srcdir)/src/bin/evfs_server_handle.c \
                        $(top_srcdir)/src/common/evfs_common.c \
                        $(top_srcdir)/src/common/evfs_command.c \
                        $(top_srcdir)/src/common/evfs_filereference.c

gzip_la_SOURCES = evfs_fs_gzip.c \
                        $(top_srcdir)/src/common/evfs_debug.c \
                        $(top_srcdir)/src/common/evfs_event_helper.c \
                        $(top_srcdir)/src/bin/evfs_server_handle.c \
                        $(top_srcdir)/src/common/evfs_common.c \
                        $(top_srcdir)/src/common/evfs_command.c \
                        $(top_srcdir)/src/common/evfs_filereference.c


samba_la_SOURCES = evfs_fs_samba.c \
                        $(top_srcdir)/src/common/evfs_debug.c \
                        $(top_srcdir)/src/common/evfs_event_helper.c \
			$(top_srcdir)/src/common/evfs_misc.c \
                        $(top_srcdir)/src/common/evfs_command.c \
                        $(top_srcdir)/src/common/evfs_filereference.c

ftp_la_SOURCES = evfs_fs_ftp.c \
                        $(top_srcdir)/src/common/evfs_debug.c \
                        $(top_srcdir)/src/common/evfs_event_helper.c \
                        $(top_srcdir)/src/common/evfs_command.c \
                        $(top_srcdir)/src/common/evfs_filereference.c

	
posix_la_CFLAGS =  @ECORE_CFLAGS@ @EFREET_CFLAGS@
posix_la_LDFLAGS = -module -avoid-version
posix_la_LIBADD =  @ECORE_LIBS@ -L$(top_builddir)/src/lib -levfs

vfolder_la_CFLAGS =  @ECORE_CFLAGS@ @EFREET_CFLAGS@
vfolder_la_LDFLAGS = -module -avoid-version
vfolder_la_LIBADD =  @ECORE_LIBS@ -L$(top_builddir)/src/lib -levfs

samba_la_CFLAGS =  @ECORE_CFLAGS@ @EFREET_CFLAGS@
samba_la_LDFLAGS = -module -avoid-version -lsmbclient
samba_la_LIBADD =  @ECORE_LIBS@ -L$(top_builddir)/src/lib -levfs

tar_la_CFLAGS =  @ECORE_CFLAGS@ @EFREET_CFLAGS@
tar_la_LDFLAGS = -module -avoid-version
tar_la_LIBADD =  @ECORE_LIBS@ -L$(top_builddir)/src/lib -levfs

bzip2_la_CFLAGS =  @ECORE_CFLAGS@ @EFREET_CFLAGS@ 
bzip2_la_LDFLAGS = -module -avoid-version
bzip2_la_LIBADD =  @ECORE_LIBS@ -L$(top_builddir)/src/lib -levfs @BZ2_LIBS@

gzip_la_CFLAGS =  @ECORE_CFLAGS@ @EFREET_CFLAGS@ 
gzip_la_LDFLAGS = -module -avoid-version
gzip_la_LIBADD =  @ECORE_LIBS@ -L$(top_builddir)/src/lib -levfs -lz


ftp_la_CFLAGS =  @ECORE_CFLAGS@ @curl_cflags@ @EFREET_CFLAGS@
ftp_la_LDFLAGS = -module -avoid-version
ftp_la_LIBADD =  @ECORE_LIBS@ -L$(top_builddir)/src/lib -levfs @curl_libs@

sftp_la_CFLAGS =  @ECORE_CFLAGS@ @EFREET_CFLAGS@
sftp_la_LDFLAGS = -module -avoid-version
sftp_la_LIBADD =  @ECORE_LIBS@ -L$(top_builddir)/src/lib -levfs

trash_la_CFLAGS =  @ECORE_CFLAGS@ @EFREET_CFLAGS@
trash_la_LDFLAGS = -module -avoid-version
trash_la_LIBADD =  @ECORE_LIBS@ -L$(top_builddir)/src/lib -levfs


