
MAINTAINERCLEANFILES = Makefile.in config.h.in stamp-h.in

if BUILD_EWL_GUI
EWLCFLAGS = \
	@EWL_CFLAGS@ 
endif

if BUILD_EDJE_GUI
EDJECFLAGS = \
	@EDJE_CFLAGS@ \
	@ESMART_CFLAGS@
endif

AM_CFLAGS = \
	@XML2_CFLAGS@ \
	@ECORE_CFLAGS@ \
	-g -O0 -Wall -W  \
	$(EDJECFLAGS) $(EWLCFLAGS)

bin_PROGRAMS = eke

if BUILD_EWL_GUI
EWLSOURCES = \
	eke_gui_ewl.c
endif

if BUILD_EDJE_GUI
EDJESOURCES = \
	eke_gui_edje.c \
	eke_gui_edje_item.c 
endif

eke_SOURCES = \
	eke_crypt.c \
	eke_file.c \
	eke_feed_item.c \
	eke_parse.c \
	eke_feed.c\
	eke_gui.c \
	main.c \
	$(EWLSOURCES) $(EDJESOURCES)

EWLLDADD = \
	@EWL_LIBS@

EDJELDADD = \
	@EDJE_LIBS@ \
	@ESMART_LIBS@ \
	-lesmart_container

eke_LDADD = \
	@XML2_LIBS@ \
	@ECORE_LIBS@ \
	-lcrypto \
	$(EWLLDADD) $(EDJELDADD)


