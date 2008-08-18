## Process this file with automake to produce Makefile.in

INCLUDES = @EDJE_CFLAGS@ \
	   @ECORE_CFLAGS@ \
	   @EVAS_CFLAGS@ \
	   @ESMART_CFLAGS@

AM_CFLAGS = -W -Wall -g -O0

bin_PROGRAMS = express

express_SOURCES = \
	 	Express.h \
	 	exp_services.h \
		exp_main.c \
		exp_gui.c \
		exp_buddy.c \
		exp_message.c \
		exp_conversation.c \
		exp_services.c \
		exp_eb.c exp_eb.h

express_LDADD = @EDJE_LIBS@ \
		@ECORE_LIBS@ \
		@EVAS_LIBS@ \
	       	@ESMART_LIBS@ -lesmart_draggies -lesmart_container

