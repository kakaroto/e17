
MAINTAINERCLEANFILES = Makefile.in

filesdir = @e_config_apps_dir@
files_DATA = emblem.eap

AM_CFLAGS = \
	@ENLIGHTENMENT_CFLAGS@ \
	@ECORE_CFLAGS@ \
	@EVAS_CFLAGS@ \
	@EDJE_CFLAGS@ \
	@EPSILON_CFLAGS@ \
	@ESMART_CFLAGS@

bin_PROGRAMS = emblem


emblem_SOURCES =  \
        Emblem.h \
	emblem_ui.c \
	emblem.c \
	main.c

emblem_LDADD = \
	@ENLIGHTENMENT_LIBS@ \
	@ECORE_LIBS@ \
	@EVAS_LIBS@ \
	@ESMART_LIBS@ \
	@EDJE_LIBS@ \
	@EPSILON_LIBS@ \
	-lesmart_container

EXTRA_DIST = \
	emblem.eap
