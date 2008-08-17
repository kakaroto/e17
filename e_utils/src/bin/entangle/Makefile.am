
MAINTAINERCLEANFILES = Makefile.in

filesdir = @e_config_apps_dir@
files_DATA = entangle.eap

AM_CFLAGS = \
@ECORE_CFLAGS@ \
@EET_CFLAGS@ \
@EDJE_CFLAGS@ \
@ESMART_CFLAGS@

bin_PROGRAMS = entangle

entangle_SOURCES =  \
Entangle.h \
entangle_apps.c \
entangle_eapps.c \
entangle_ui.c \
main.c

entangle_LDADD = \
@ECORE_LIBS@ \
@EET_LIBS@ \
@EDJE_LIBS@ \
@ESMART_LIBS@ \
-lesmart_container \
-lesmart_text_entry

EXTRA_DIST = \
	     entangle.eap
