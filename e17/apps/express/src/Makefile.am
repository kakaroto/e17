## Process this file with automake to produce Makefile.in

INCLUDES = @edje_cflags@ @ecore_cflags@ @edb_cflags@ @evas_cflags@ \
@esmart_cflags@ @imlib2_cflags@ @etox_cflags@

bin_PROGRAMS = express

express_SOURCES = \
	 	Express.h \
        exp_main.c \
		exp_gui.c \
		exp_buddy.c \
		exp_message.c \
		exp_conversation.c
		


express_LDADD = @edje_libs@ @ecore_libs@ @edb_libs@ @evas_libs@ \
@esmart_libs@ @imlib2_libs@ @etox_libs@ -lesmart_draggies -lesmart_container

