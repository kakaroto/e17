
MAINTAINERCLEANFILES = Makefile.in

filesdir = @e_config_apps_dir@
files_DATA = emblem.eapp

AM_CFLAGS = \
	@e_cflags@ \
	@ecore_cflags@ \
	@evas_cflags@ \
	@edje_cflags@ \
	@esmart_cflags@

bin_PROGRAMS = emblem

emblem_SOURCES =  \
	emblem_ui.c \
	emblem.c \
	main.c

emblem_LDADD = \
	@e_libs@ \
	@ecore_libs@ \
	@evas_libs@ \
	@esmart_libs@ \
	@edje_libs@ \
	-lesmart_container

