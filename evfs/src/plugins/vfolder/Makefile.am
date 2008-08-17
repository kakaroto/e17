## Process this file with automake to produce Makefile.in
AUTOMAKE_OPTIONS     = 1.7 foreign

LDFLAGS              = -rdynamic -shared
INCLUDES             = -I. -I$(top_srcdir) -I$(top_srcdir)/src/include

pkgdir =  $(libdir)/evfs/plugins/vfolder

GROUP_L = group.la
TRASH_L = trash.la
EFOLDER_L = efolder.la

pkg_LTLIBRARIES         = $(GROUP_L) $(TRASH_L) $(EFOLDER_L)

group_la_SOURCES = evfs_vfolder_group.c \
                       $(top_srcdir)/src/common/evfs_debug.c \
                        $(top_srcdir)/src/common/evfs_event_helper.c \
                        $(top_srcdir)/src/common/evfs_misc.c

efolder_la_SOURCES = evfs_vfolder_efolder.c \
                       $(top_srcdir)/src/common/evfs_debug.c \
                        $(top_srcdir)/src/common/evfs_event_helper.c \
                        $(top_srcdir)/src/common/evfs_misc.c

trash_la_SOURCES = evfs_vfolder_trash.c \
                       $(top_srcdir)/src/common/evfs_debug.c \
                        $(top_srcdir)/src/common/evfs_event_helper.c \
                        $(top_srcdir)/src/common/evfs_misc.c

group_la_CFLAGS =  @ECORE_CFLAGS@ @EFREET_CFLAGS@
group_la_LDFLAGS = -module -avoid-version
group_la_LIBADD =  @ECORE_LIBS@ @taglib_libs@ -L$(top_builddir)/src/lib -levfs

efolder_la_CFLAGS =  @ECORE_CFLAGS@ @EFREET_CFLAGS@
efolder_la_LDFLAGS = -module -avoid-version
efolder_la_LIBADD =  @ECORE_LIBS@ @taglib_libs@ -L$(top_builddir)/src/lib -levfs

trash_la_CFLAGS =  @ECORE_CFLAGS@ @EFREET_CFLAGS@
trash_la_LDFLAGS = -module -avoid-version
trash_la_LIBADD =  @EFREET_LIBS@ @ECORE_LIBS@ @taglib_libs@ -L$(top_builddir)/src/lib -levfs


