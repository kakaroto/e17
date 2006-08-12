## Process this file with automake to produce Makefile.in
AUTOMAKE_OPTIONS     = 1.7 foreign

LDFLAGS              = -rdynamic -shared
INCLUDES             = -I. -I$(top_srcdir) -I$(top_srcdir)/src/include

pkgdir =  $(libdir)/evfs/plugins/vfolder

GROUP_L = group.la
TRASH_L = trash.la

pkg_LTLIBRARIES         = $(GROUP_L) $(TRASH_L)

group_la_SOURCES = evfs_vfolder_group.c \
                       $(top_srcdir)/src/common/evfs_debug.c \
                        $(top_srcdir)/src/common/evfs_event_helper.c \
                        $(top_srcdir)/src/common/evfs_misc.c

trash_la_SOURCES = evfs_vfolder_trash.c \
                       $(top_srcdir)/src/common/evfs_debug.c \
                        $(top_srcdir)/src/common/evfs_event_helper.c \
                        $(top_srcdir)/src/common/evfs_misc.c

group_la_CFLAGS =  @ecore_cflags@ 
group_la_LDFLAGS = -module -avoid-version
group_la_LIBADD =  @ecore_libs@ @taglib_libs@ -L$(top_builddir)/src/lib -levfs

trash_la_CFLAGS =  @ecore_cflags@ 
trash_la_LDFLAGS = -module -avoid-version
trash_la_LIBADD =  @ecore_libs@ @taglib_libs@ -L$(top_builddir)/src/lib -levfs


