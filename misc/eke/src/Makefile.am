
MAINTAINERCLEANFILES = Makefile.in config.h.in stamp-h.in

AM_CFLAGS = \
	@XML2_CFLAGS@ \
	@EWL_CFLAGS@ \
	@EDJE_CFLAGS@ \
	@ETOX_CFLAGS@ \
	@ESMART_CFLAGS@ \
	@ECORE_CFLAGS@ \
	-g -O0 -Wall -W

bin_PROGRAMS = eke

eke_SOURCES = \
	eke_crypt.c \
	eke_file.c \
	eke_feed_item.c \
	eke_parse.c \
	eke_feed.c\
	eke_gui_ewl.c \
	eke_gui_edje.c \
	eke_gui_edje_item.c \
	eke_gui.c \
	main.c

eke_LDADD = \
	@XML2_LIBS@ \
	@EWL_LIBS@ \
	@EDJE_LIBS@ \
	@ETOX_LIBS@ \
	@ESMART_LIBS@ \
	@ECORE_LIBS@ \
	-lesmart_container -lcrypto


