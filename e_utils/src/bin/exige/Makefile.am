
MAINTAINERCLEANFILES = Makefile.in

AM_CFLAGS = \
	@ECORE_CFLAGS@ \
	@EVAS_CFLAGS@ \
	@EDJE_CFLAGS@ \
	@EET_CFLAGS@ \
	@ESMART_CFLAGS@

bin_PROGRAMS = exige

exige_SOURCES = \
	exg_callback.h \
	exg_conf.h \
	exg_gui.h \
	Exige.h \
	exg_conf.c \
	exg_callback.c \
	exg_gui.c \
	exg_main.c

exige_LDADD = \
	@ECORE_LIBS@ \
	@EET_LIBS@ \
	@EVAS_LIBS@ \
	@ESMART_LIBS@ \
	@EDJE_LIBS@ \
	-lesmart_text_entry

#-lesmart_textarea 
