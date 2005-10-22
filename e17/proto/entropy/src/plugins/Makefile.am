## Process this file with automake to produce Makefile.in
AUTOMAKE_OPTIONS     = 1.7 foreign

LDFLAGS              = -rdynamic -shared
INCLUDES             = -I. -I$(top_srcdir) -I$(top_srcdir)/src/include

pkgdir =  $(datadir)/entropy/plugins

MIME_L = mime.la
THUMBNAILER_L = imlib_thumbnailer.la
POSIX_L = posix.la
LAYOUT_L = layout_ewl_simple.la
EWL_LOCAL_ICON_L = ewl_local_icon.la
STRUCTURE_VIEWER_L = structure_viewer.la
SYSTEM_THUMBNAILER_L = system_thumbnailer.la
ACTION_SIMPLE_L = action_simple.la

#if 0
#LAYOUT_ETK_L = layout_etk_simple.la
#else
LAYOUT_ETK_L =
#endif

pkg_LTLIBRARIES		= $(MIME_L) $(THUMBNAILER_L) $(POSIX_L) $(LAYOUT_L) $(EWL_LOCAL_ICON_L) $(STRUCTURE_VIEWER_L) $(SYSTEM_THUMBNAILER_L) $(ACTION_SIMPLE_L) $(LAYOUT_ETK_L)

ewl_local_icon_la_SOURCES      = ewl_icon_local_viewer.c ../entropy_debug.c
ewl_local_icon_la_CFLAGS       = @ewl_cflags@
ewl_local_icon_la_LDFLAGS      = -module -avoid-version
ewl_local_icon_la_LIBADD       = @ewl_libs@

structure_viewer_la_SOURCES    = structure_viewer.c ../entropy_debug.c
structure_viewer_la_CFLAGS     = @ewl_cflags@
structure_viewer_la_LDFLAGS    = -module -avoid-version
structure_viewer_la_LIBADD     = @ewl_libs@

action_simple_la_SOURCES       = action_simple.c ../entropy_debug.c
action_simple_la_CFLAGS        = @ewl_cflags@
action_simple_la_LDFLAGS       = -module -avoid-version
action_simple_la_LIBADD        = @ewl_libs@


layout_ewl_simple_la_SOURCES   = layout_ewl_simple.c ../entropy_debug.c
layout_ewl_simple_la_CFLAGS    = @ewl_cflags@
layout_ewl_simple_la_LDFLAGS   = -module -avoid-version
layout_ewl_simple_la_LIBADD    = @ewl_libs@

#layout_etk_simple_la_SOURCES  =  layout_etk_simple.c ../entropy_debug.c
#layout_etk_simple_la_CFLAGS   = @etk_cflags@
#layout_etk_simple_la_LDFLAGS  = -module -avoid-version
#layout_etk_simple_la_LIBADD   = @etk_libs@


mime_la_SOURCES                = mime.c ../entropy_core.c
mime_la_CFLAGS 	               = @evfs_cflags@
mime_la_LDFLAGS                = -module -avoid-version

imlib_thumbnailer_la_SOURCES   = thumbnailer.c thumbnail_generic.c md5.c ../entropy_debug.c
imlib_thumbnailer_la_CFLAGS    = @ecore_cflags@
imlib_thumbnailer_la_LDFLAGS   = -module -avoid-version
imlib_thumbnailer_la_LIBADD    = @imlib2_libs@ @ecore_libs@ @png_libs@

system_thumbnailer_la_SOURCES  = thumbnail_system.c thumbnail_generic.c ../entropy_debug.c
system_thumbnailer_la_CFLAGS   = @ecore_cflags@
system_thumbnailer_la_LDFLAGS  = -module -avoid-version
system_thumbnailer_la_LIBADD   = @ecore_libs@


posix_la_SOURCES               = filesystem.c ../entropy_debug.c
posix_la_CFLAGS                = @evfs_cflags@
posix_la_LDFLAGS               = -module -avoid-version
posix_la_LIBADD                = @evfs_libs@

