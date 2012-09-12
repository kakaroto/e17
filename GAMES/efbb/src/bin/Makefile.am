MAINTAINERCLEANFILES = Makefile.in

INCLUDES = \
-I$(top_srcdir) \
-I$(top_srcdir)/src/bin \
-DPACKAGE_BIN_DIR=\"$(bindir)\" \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
@EFBB_CFLAGS@

game_world.c game_world.h: game_world.geneet
	geneet.py $(top_srcdir)/src/bin/game_world.geneet

bin_PROGRAMS = efbb
if BUILD_QUICKLAUNCH
bin_PROGRAMS += efbb_ql
endif

GAME_SOURCES = \
	$(builddir)/game_world.c \
	main.c \
	main_screen.c \
	level_select_screen.c \
	cannon.c \
	sound.c \
        score.c \
	bg.c

efbb_LDADD = @EFBB_LIBS@
efbb_SOURCES = $(GAME_SOURCES)

if BUILD_QUICKLAUNCH
############################################################################
## Build quick launch binary, needs elementary_quicklaunch to be enabled. ##
##                                                                        ##
## It is composed of a library with actual code and a binary that talks   ##
## to server that will then fork() + dlopen() such library.               ##
############################################################################
efbb_qldir = $(quicklauncher_libdir)
efbb_ql_LTLIBRARIES = efbb_ql.la
efbb_ql_la_SOURCES = $(GAME_SOURCES)
efbb_ql_la_LIBADD = @EFBB_LIBS@
efbb_ql_la_CFLAGS =
efbb_ql_la_LDFLAGS = -module -avoid-version -no-undefined
efbb_ql_SOURCES = main.c
efbb_ql_LDADD = @EFBB_LIBS@
efbb_ql_CFLAGS = -DELM_LIB_QUICKLAUNCH=1
efbb_ql_LDFLAGS =
endif

noinst_HEADERS = efbb.h $(builddir)/game_world.h main_screen.h level_select_screen.h cannon.h sound.h score.h bg.h
EXTRA_DIST = $(noinst_HEADERS) *.geneet
