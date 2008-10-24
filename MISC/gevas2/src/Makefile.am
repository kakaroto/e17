## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS     = 1.4 foreign

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = Makefile.in

LIBS_EVAS     = @evas_libs@
LIBS_EXTRA    = @edb_libs@ @EDJE_LIBS@ @EVAS_LIBS@


CFLAGS_EVAS   = @evas_cflags@ @EDJE_CFLAGS@ @EVAS_CFLAGS@
CFLAGS_EXTRA  = -I$(top_srcdir) @edb_cflags@

LIBS_ALL      = $(LIBS_EVAS) $(LIBS_EXTRA)
CFLAGS_ALL    = $(CFLAGS_EVAS) $(CFLAGS_EXTRA)

INCLUDES	     = $(CFLAGS_ALL)           \
			    @GTK_CFLAGS@           \
		       -DLIBDIR=\"$(libdir)\"  \
		       -DBINDIR=\"$(bindir)\"  \
		       -DDATADIR=\"$(datadir)\"

lib_LTLIBRARIES      = libgevas.la
include_HEADERS      = \
gevas.h                   \
gevasev_handler.h         \
gevasevh_alpha.h          \
gevasevh_clicks.h         \
gevasevh_drag.h           \
gevasevh_emouse_over.h    \
gevasevh_throb.h          \
gevasevh_obj_changer.h    \
gevasevh_popup.h          \
gevasevh_to_gtk_signals.h \
gevasgrad.h               \
gevasimage.h              \
gevasedje.h               \
gevasobj.h                \
gevastext.h               \
gevastwin.h               \
gevasevh_selectable.h     \
gevasevh_group_selector.h \
gevas_obj_collection.h    \
gevas_sprite.h            \
gevas_util.h              \
gevastrans_alphawipe.h

libgevas_la_SOURCES   =   \
gevas.c                   \
gevasev_handler.c         \
gevasevh_alpha.c          \
gevasevh_clicks.c         \
gevasevh_drag.c           \
gevasevh_emouse_over.c    \
gevasevh_throb.c          \
gevasevh_obj_changer.c    \
gevasevh_popup.c          \
gevasevh_to_gtk_signals.c \
gevasgrad.c               \
gevasimage.c              \
gevasedje.c               \
gevasobj.c                \
gevastext.c               \
gevastwin.c               \
gevasevh_selectable.c     \
gevasevh_group_selector.c \
gevas_obj_collection.c    \
gevas_sprite.c            \
gevas_util.c              \
project.c                 \
project.h                 \
gevastrans_alphawipe.c    

libgevas_la_LIBADD       = $(LIBS_ALL)  @GTK_LIBS@
libgevas_la_LDFLAGS      = -version-info 1:0:0
