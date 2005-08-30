## Process this file with automake to produce Makefile.in

INCLUDES = @edje_cflags@ @ecore_cflags@ @edb_cflags@ @evas_cflags@ \
@esmart_cflags@ @imlib2_cflags@ @etox_cflags@

CFLAGS = -W -Wall -g -O0

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

express_LDADD = @edje_libs@ @ecore_libs@ @edb_libs@ @evas_libs@ \
@esmart_libs@ @imlib2_libs@ @etox_libs@ -lesmart_draggies -lesmart_container

