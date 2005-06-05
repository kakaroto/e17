
MAINTAINERCLEANFILES = Makefile.in

AM_CFLAGS = \
	@ecore_cflags@ \
	@evas_cflags@ \
	@edje_cflags@ \
	@esmart_cflags@

bin_PROGRAMS = exige

exige_SOURCES = \
	exg_conf.c \
	exg_callback.c \
	exg_gui.c \
	exg_main.c

exige_LDADD = \
	@ecore_libs@ \
	@evas_libs@ \
	@esmart_libs@ \
	@edje_libs@ \
	-lesmart_textarea \
	-lesmart_text_entry

