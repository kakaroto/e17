## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = \
-DPACKAGE_LIB_DIR=\"$(libdir)\" \
-DPACKAGE_BIN_DIR=\"$(bindir)\" \
-DPACKAGE_DATA_DIR=\"$(datadir)\" \
-DLOCALEDIR=\"$(datadir)/locale\" \
@ENNA_CFLAGS@

bin_PROGRAMS = enna-explorer

enna_explorer_SOURCES = \
activity.c\
browser.c\
browser_bar.c\
browser_obj.c\
buffer.c\
enna.c\
enna_config.c\
explorer.c\
file.c\
localfiles.c \
mainmenu.c\
shortcut.c\
udisks.c\
utils.c\
vfs.c\
view_list.c\
view_grid.c

enna_explorer_LDADD = @ENNA_LIBS@ -lm
enna_explorer_LDFLAGS = -rdynamic

EXTRA_DIST= \
activity.h\
browser.h\
browser_obj.h\
buffer.h \
enna.h\
enna_config.h\
explorer.h\
file.h\
gettext.h \
localfiles.h \
mainmenu.h\
module.h \
shortcut.h\
utils.h\
vfs.h\
view_list.h\
view_grid.h