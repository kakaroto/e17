## Process this file with automake to produce Makefile.in
AUTOMAKE_OPTIONS     = 1.7 foreign

LDFLAGS              = -rdynamic -shared
INCLUDES             = -I. -I$(top_srcdir) -I$(top_srcdir)/src/include

pkgdir =  $(datadir)/entropy/plugins

MIME_L = mime.la
THUMBNAILER_L = imlib_thumbnailer.la
REMOTE_THUMBNAILER_L = remote_thumbnailer.la
DISTRIB_THUMBNAILER_L = distrib_thumbnailer.la
POSIX_L = posix.la

#if HAVE_EWL
#LAYOUT_L = layout_ewl_simple.la
#EWL_LOCAL_ICON_L = ewl_local_icon.la
#EWL_LIST_L = ewl_list.la
#STRUCTURE_VIEWER_L = structure_viewer.la
#else
LAYOUT_L =
EWL_LOCAL_ICON_L =
EWL_LIST_L =
STRUCTURE_VIEWER_L =
#endif

SYSTEM_THUMBNAILER_L = system_thumbnailer.la
ACTION_SIMPLE_L = action_simple.la

if HAVE_EXTRACTOR
METADATA_EXTRACT_L = metadata_extract.la
else
METADATA_EXTRACT_L =
endif

if HAVE_ETK
LAYOUT_ETK_L = layout_etk_simple.la
ETK_STRUCTURE_VIEWER_L = etk_structure_viewer.la
ETK_LIST_VIEWER_L = etk_list.la
ETK_METADATA_L = etk_metadata.la
ETK_ICONBOX_L = etk_iconbox.la
ETK_TRACKBACK_L = etk_trackback.la

if HAVE_EMOTION
ETK_HOVER_L = etk_hover.la
else
ETK_HOVER_L =
endif

else
LAYOUT_ETK_L =
ETK_LIST_VIEWER_L =
ETK_STRUCTURE_VIEWER_L =
ETK_METADATA_L =
ETK_ICONBOX_L = 
ETK_TRACKBACK_L =
ETK_HOVER_L =
endif

pkg_LTLIBRARIES		= $(MIME_L) $(THUMBNAILER_L) $(POSIX_L) $(LAYOUT_L) $(EWL_LOCAL_ICON_L) $(STRUCTURE_VIEWER_L) $(SYSTEM_THUMBNAILER_L) $(ACTION_SIMPLE_L) $(LAYOUT_ETK_L) $(EWL_LIST_L) $(REMOTE_THUMBNAILER_L) $(DISTRIB_THUMBNAILER_L) $(ETK_STRUCTURE_VIEWER_L) $(ETK_LIST_VIEWER_L) $(METADATA_EXTRACT_L) $(ETK_METADATA_L) $(ETK_ICONBOX_L) $(ETK_TRACKBACK_L) $(ETK_HOVER_L)

ewl_local_icon_la_SOURCES      = 	$(top_srcdir)/src/entropy_gui.c \
					$(top_srcdir)/src/dialogs/ewl_progress_dialog.c \
					$(top_srcdir)/src/dialogs/ewl_rename_dialog.c \
					$(top_srcdir)/src/dialogs/ewl_interaction_dialog.c \
					ewl_icon_local_viewer.c $(top_srcdir)/src/dialogs/ewl_properties_dialog.c \
					$(top_srcdir)/src/entropy_debug.c
ewl_local_icon_la_CFLAGS       = @EWL_CFLAGS@
ewl_local_icon_la_LDFLAGS      = -module -avoid-version
ewl_local_icon_la_LIBADD       = @EWL_LIBS@

ewl_list_la_SOURCES      =        $(top_srcdir)/src/entropy_gui.c \
                                        $(top_srcdir)/src/dialogs/ewl_progress_dialog.c \
					$(top_srcdir)/src/dialogs/ewl_rename_dialog.c \
                                        ewl_list_viewer.c $(top_srcdir)/src/dialogs/ewl_properties_dialog.c \
					$(top_srcdir)/src/dialogs/ewl_interaction_dialog.c \
					$(top_srcdir)/src/entropy_debug.c
ewl_list_la_CFLAGS       = @EWL_CFLAGS@
ewl_list_la_LDFLAGS      = -module -avoid-version
ewl_list_la_LIBADD       = @EWL_LIBS@

metadata_extract_la_SOURCES = metadata_extract.c
metadata_extract_la_CFLAGS = @extractor_cflags@
metadata_extract_la_LDFLAGS = -module -avoid-version
metadata_extract_la_LIBADD = @extractor_libs@


structure_viewer_la_SOURCES    = $(top_srcdir)/src/entropy_gui.c \
				structure_viewer.c $(top_srcdir)/src/entropy_debug.c
structure_viewer_la_CFLAGS     = @EWL_CFLAGS@
structure_viewer_la_LDFLAGS    = -module -avoid-version
structure_viewer_la_LIBADD     = @EWL_LIBS@

action_simple_la_SOURCES       = $(top_srcdir)/src/entropy_gui.c \
				action_simple.c $(top_srcdir)/src/entropy_debug.c
action_simple_la_CFLAGS        = @ECORE_CFLAGS@ @EWL_CFLAGS@
action_simple_la_LDFLAGS       = -module -avoid-version
action_simple_la_LIBADD        = @ECORE_LIBS@ @EWL_LIBS@


layout_ewl_simple_la_SOURCES   = $(top_srcdir)/src/entropy_gui.c \
				$(top_srcdir)/src/dialogs/ewl_mime_dialog.c layout_ewl_simple.c $(top_srcdir)/src/entropy_debug.c \
				 $(top_srcdir)/src/dialogs/ewl_tip.c \
				 $(top_srcdir)/src/dialogs/ewl_about_dialog.c
layout_ewl_simple_la_CFLAGS    = @EWL_CFLAGS@
layout_ewl_simple_la_LDFLAGS   = -module -avoid-version
layout_ewl_simple_la_LIBADD    = @EWL_LIBS@

if HAVE_ETK
layout_etk_simple_la_SOURCES  =  layout_etk_simple.c $(top_srcdir)/src/entropy_debug.c \
                                $(top_srcdir)/src/dialogs/etk_progress_dialog.c \
                                $(top_srcdir)/src/dialogs/etk_interaction_dialog.c \
				$(top_srcdir)/src/dialogs/etk_location_add_dialog.c \
				$(top_srcdir)/src/dialogs/etk_file_context_menu.c \
				$(top_srcdir)/src/dialogs/etk_mime_dialog_main.c \
                                $(top_srcdir)/src/dialogs/etk_properties_dialog.c \
				$(top_srcdir)/src/dialogs/etk_options_dialog.c \
				$(top_srcdir)/src/dialogs/etk_file_cache_debug_dialog.c \
				$(top_srcdir)/src/dialogs/etk_auth_request_dialog.c \
				$(top_srcdir)/src/dialogs/etk_efolder_dialog.c \
				$(top_srcdir)/src/dialogs/etk_about_dialog.c

layout_etk_simple_la_CFLAGS   = @ETK_CFLAGS@
layout_etk_simple_la_LDFLAGS  = -module -avoid-version
layout_etk_simple_la_LIBADD   = @ETK_LIBS@

etk_structure_viewer_la_SOURCES    = $(top_srcdir)/src/entropy_gui.c \
                                $(top_srcdir)/src/dialogs/etk_directory_add_dialog.c \
                                $(top_srcdir)/src/dialogs/etk_properties_dialog.c \
                                $(top_srcdir)/src/dialogs/etk_file_context_menu.c \
                                $(top_srcdir)/src/dialogs/etk_file_rename_dialog.c \
                                etk_structure_viewer.c $(top_srcdir)/src/entropy_debug.c
etk_structure_viewer_la_CFLAGS     = @ETK_CFLAGS@
etk_structure_viewer_la_LDFLAGS    = -module -avoid-version
etk_structure_viewer_la_LIBADD     = @ETK_LIBS@

etk_list_la_SOURCES      =      etk_list_viewer.c \
				$(top_srcdir)/src/entropy_gui.c \
                                $(top_srcdir)/src/entropy_debug.c \
				$(top_srcdir)/src/dialogs/etk_directory_add_dialog.c \
				$(top_srcdir)/src/dialogs/etk_properties_dialog.c \
				$(top_srcdir)/src/dialogs/etk_file_context_menu.c \
				$(top_srcdir)/src/dialogs/etk_file_rename_dialog.c \
				$(top_srcdir)/src/dialogs/etk_interaction_dialog.c
etk_list_la_CFLAGS       = @ETK_CFLAGS@
etk_list_la_LDFLAGS      = -module -avoid-version
etk_list_la_LIBADD       = @ETK_LIBS@

etk_iconbox_la_SOURCES      =      etk_icon_viewer.c \
                                $(top_srcdir)/src/entropy_gui.c \
                                $(top_srcdir)/src/entropy_debug.c \
                                $(top_srcdir)/src/dialogs/etk_directory_add_dialog.c \
                                $(top_srcdir)/src/dialogs/etk_file_context_menu.c \
				$(top_srcdir)/src/dialogs/etk_file_rename_dialog.c \
                                $(top_srcdir)/src/dialogs/etk_properties_dialog.c \
				$(top_srcdir)/src/dialogs/etk_interaction_dialog.c
etk_iconbox_la_CFLAGS       = @ETK_CFLAGS@
etk_iconbox_la_LDFLAGS      = -module -avoid-version
etk_iconbox_la_LIBADD       = @ETK_LIBS@


etk_trackback_la_SOURCES = etk_trackback.c \
                                $(top_srcdir)/src/entropy_gui.c \
                                $(top_srcdir)/src/entropy_debug.c 
etk_trackback_la_CFLAGS	= @ETK_CFLAGS@
etk_trackback_la_LDFLAGS = -module -avoid-version
etk_trackback_la_LIBADD	= @ETK_LIBS@



etk_metadata_la_SOURCES = etk_meta_plugin.c
etk_metadata_la_CFLAGS       = @ETK_CFLAGS@
etk_metadata_la_LDFLAGS      = -module -avoid-version
etk_metadata_la_LIBADD       = @ETK_LIBS@

if HAVE_EMOTION
etk_hover_la_SOURCES = etk_hover_viewer.c
etk_hover_la_CFLAGS = @ETK_CFLAGS@ @EMOTION_CFLAGS@
etk_hover_la_LDFLAGS = -module -avoid-version
etk_hover_la_LIBADD = @ETK_LIBS@ @EMOTION_LIBS@
endif

endif

mime_la_SOURCES                = mime.c $(top_srcdir)/src/entropy_core.c
mime_la_CFLAGS 	               = @EVFS_CFLAGS@ 
mime_la_LDFLAGS                = -module -avoid-version
mime_la_LIBADD		       = 

imlib_thumbnailer_la_SOURCES   = thumbnailer.c thumbnail_generic.c md5.c $(top_srcdir)/src/entropy_debug.c
imlib_thumbnailer_la_CFLAGS    = @ECORE_CFLAGS@ @IMLIB2_CFLAGS@ @EPSILON_CFLAGS@  
imlib_thumbnailer_la_LDFLAGS   = -module -avoid-version
imlib_thumbnailer_la_LIBADD    = @IMLIB2_LIBS@ @ECORE_LIBS@ @PNG_LIBS@ @EPSILON_LIBS@  

remote_thumbnailer_la_SOURCES   = remote_thumbnailer.c thumbnail_generic.c md5.c $(top_srcdir)/src/entropy_debug.c
remote_thumbnailer_la_CFLAGS    = @ECORE_CFLAGS@ @IMLIB2_CFLAGS@ @EPSILON_CFLAGS@  
remote_thumbnailer_la_LDFLAGS   = -module -avoid-version
remote_thumbnailer_la_LIBADD    = @IMLIB2_LIBS@ @ECORE_LIBS@ @PNG_LIBS@ @EPSILON_LIBS@  

distrib_thumbnailer_la_SOURCES   = distribution_thumbnailer.c thumbnail_generic.c md5.c $(top_srcdir)/src/entropy_debug.c
distrib_thumbnailer_la_CFLAGS    = @ECORE_CFLAGS@ @IMLIB2_CFLAGS@ @EPSILON_CFLAGS@  
distrib_thumbnailer_la_LDFLAGS   = -module -avoid-version
distrib_thumbnailer_la_LIBADD    = @IMLIB2_LIBS@ @ECORE_LIBS@ @PNG_LIBS@ @EPSILON_LIBS@  


system_thumbnailer_la_SOURCES  = thumbnail_system.c thumbnail_generic.c $(top_srcdir)/src/entropy_debug.c
system_thumbnailer_la_CFLAGS   = @ECORE_CFLAGS@ @IMLIB2_CFLAGS@ @EPSILON_CFLAGS@  
system_thumbnailer_la_LDFLAGS  = -module -avoid-version
system_thumbnailer_la_LIBADD   = @ECORE_LIBS@ @EPSILON_LIBS@  


posix_la_SOURCES               = filesystem.c $(top_srcdir)/src/entropy_debug.c
posix_la_CFLAGS                = @EVFS_CFLAGS@  
posix_la_LDFLAGS               = -module -avoid-version
posix_la_LIBADD                = @EVFS_LIBS@  

