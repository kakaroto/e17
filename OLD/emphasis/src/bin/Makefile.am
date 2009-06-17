## Process this file with automake to produce Makefile.in

AM_CFLAGS = \
-Wall -W -g -D_GNU_SOURCE @EMPHASIS_CFLAGS@

bin_PROGRAMS = emphasis

emphasis_INCLUDES = \
emphasis.h            \
emphasis_mpc.h        \
emphasis_player.h     \
emphasis_config.h     \
emphasis_config_gui.h \
emphasis_gui.h        \
emphasis_media.h      \
emphasis_callbacks.h  \
emphasis_misc.h       \
emphasis_cover.h      \
emphasis_cover_db.h   \
emphasis_convert.h    \
emphasis_data_list.h  \
emphasis_search.h

emphasis_SOURCES = \
emphasis.c            \
emphasis_mpc.c        \
emphasis_player.c     \
emphasis_config.c     \
emphasis_config_gui.c \
emphasis_gui.c        \
emphasis_media.c      \
emphasis_callbacks.c  \
emphasis_misc.c       \
emphasis_cover.c      \
emphasis_cover_db.c   \
emphasis_convert.c    \
emphasis_data_list.c  \
emphasis_search.c

emphasis_LDFLAGS = @DLLDFLAGS@
emphasis_LDADD = @EMPHASIS_LIBS@ 

MAINTAINERCLEANFILES = Makefile.in
