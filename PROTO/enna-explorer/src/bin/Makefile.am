## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = \
-DPACKAGE_LIB_DIR=\"$(libdir)\" \
-DPACKAGE_BIN_DIR=\"$(bindir)\" \
-DPACKAGE_DATA_DIR=\"$(datadir)\" \
-DLOCALEDIR=\"$(datadir)/locale\" \
@ENNA_CFLAGS@

bin_PROGRAMS = enna

enna_SOURCES = \
enna.c\
enna_config.c\
explorer.c\
shortcut.c\
utils.c\
buffer.c\
mainmenu.c\
module.c\
activity.c\
vfs.c\
volumes.c\
file.c\
browser.c\
browser_obj.c\
view_list.c\
view_grid.c

enna_LDADD = @ENNA_LIBS@
enna_LDFLAGS = -rdynamic


##########################################################################
# For Modules Static linking : BEGIN
##########################################################################

if USE_STATIC_MODULES
include ../modules/browser/localfiles/config.mak
include ../modules/browser/upnp/config.mak
include ../modules/volume/mtab/config.mak

if BUILD_ACTIVITY_EXPLORER
enna_SOURCES += $(SRCS_ACTIVITY_EXPLORER)
endif

if BUILD_BROWSER_LOCALFILES
enna_SOURCES += $(SRCS_BROWSER_LOCALFILES)
endif

if BUILD_BROWSER_UPNP
enna_SOURCES += $(SRCS_BROWSER_UPNP)
AM_CPPFLAGS += @GUPNP_CFLAGS@
enna_LDADD += @GUPNP_LIBS@
endif

if BUILD_MODULE_VOLUME_MTAB
enna_SOURCES += $(top_srcdir)/src/modules/volume/mtab/mtab.c
endif

if BUILD_MODULE_VOLUME_UDEV
enna_SOURCES += $(top_srcdir)/src/modules/volume/udev/udev.c
AM_CPPFLAGS += @enna_module_volume_udev_cflags@
enna_LDADD += @enna_module_volume_udev_libs@
endif

endif

##########################################################################
# For Modules Static linking : END
##########################################################################

EXTRA_DIST= \
activity.h \
browser.h \
browser_obj.h \
buffer.h \
enna_config.h \
enna.h \
explorer.h \
file.h \
mainmenu.h \
module.h \
shortcut.h \
utils.h \
vfs.h \
view_grid.h \
view_list.h \
volumes.h
