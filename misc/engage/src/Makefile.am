## Process this file with automake to produce Makefile.in
SUBDIRS=module

INCLUDES = \
-I../lib \
-I/usr/local/include \
-I$(top_srcdir)/lib \
@evas_cflags@ \
@edje_cflags@ \
@ecore_cflags@ \
@esmart_cflags@ \
@ewl_cflags@ \
@imlib2_cflags@ \
@e_cflags@

bin_PROGRAMS = engage

engage_SOURCES = \
e.h \
e_apps.h \
e_file.h \
e_menu.h \
e_object.h \
e_path.h \
e_user.h \
engage.h \
engage_element.h \
main.c \
config.c \
dock.c \
icon.c \
wm.c \
window.c \
tray.c \
userconfig.c \
e_object.c \
e_file.c \
e_user.c \
e_apps.c

if XINERAMA
engage_LDADD = @evas_libs@ @edje_libs@ @ecore_libs@ @imlib2_libs@ @ewl_libs@ \
					@esmart_libs@ -lesmart_trans_x11 -lXinerama
engage_CFLAGS = -DXINERAMA
else
engage_LDADD = @evas_libs@ @edje_libs@ @ecore_libs@ @imlib2_libs@ @ewl_libs@ \
          @esmart_libs@ -lesmart_trans_x11
endif
