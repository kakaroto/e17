## Process this file with automake to produce Makefile.in

INCLUDES = \
-I../lib \
-I/usr/local/include \
-I$(top_srcdir)/lib @ENGAGE_CFLAGS@

bin_PROGRAMS = engage

engage_SOURCES = \
e.h \
e_includes.h \
e_apps.h \
e_apps_cache.h \
e_icon.h \
e_intl.h \
e_object.h \
e_path.h \
e_user.h \
e_utils.h \
engage.h \
order.h \
engage_element.h \
battery.h \
main.c \
order.c \
config.c \
config_menu.c \
dock.c \
icon.c \
wm.c \
window.c \
tray.c \
userconfig.c \
battery.c \
e_object.c \
e_path.c \
e_user.c \
e_utils.c \
e_apps.c \
e_apps_cache.c \
e_icon.c \
e_intl.c

if XINERAMA
engage_LDADD = @ENGAGE_LIBS@ -lesmart_trans_x11 -lXinerama
engage_CFLAGS = -DXINERAMA
else
engage_LDADD = @ENGAGE_LIBS@ -lesmart_trans_x11 
endif
