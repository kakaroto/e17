
MAINTAINERCLEANFILES = Makefile.in

filesdir = @e_config_apps_dir@
files_DATA = entangle.eap

AM_CFLAGS = \
@ecore_cflags@ \
@eet_cflags@ \
@edje_cflags@ \
@esmart_cflags@

bin_PROGRAMS = entangle

entangle_SOURCES =  \
entangle_apps.c \
entangle_eapps.c \
entangle_ui.c \
main.c

entangle_LDADD = \
@ecore_libs@ \
@eet_libs@ \
@edje_libs@ \
@esmart_libs@ \
-lesmart_container \
-lesmart_text_entry

