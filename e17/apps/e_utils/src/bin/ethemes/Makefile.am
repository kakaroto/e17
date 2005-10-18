
MAINTAINERCLEANFILES = Makefile.in

filesdir = @e_config_apps_dir@
files_DATA = ethemes.eap

AM_CFLAGS = \
	-DE17PREFIX=\"`enlightenment-config --prefix`\" \
	@ECORE_CFLAGS@ \
	@EVAS_CFLAGS@ \
	@EDJE_CFLAGS@ \
	@ENLIGHTENMENT_CFLAGS@


bin_PROGRAMS = ethemes

ethemes_SOURCES = \
	E_Preview.h \
	e_preview.c \
	main.c

ethemes_LDADD = \
	@ECORE_LIBS@ \
	@EVAS_LIBS@ \
	@EDJE_LIBS@ \
	@ESMART_LIBS@ \
	-lesmart_container \
	@ENLIGHTENMENT_LIBS@

EXTRA_DIST = \
	ethemes.eap

