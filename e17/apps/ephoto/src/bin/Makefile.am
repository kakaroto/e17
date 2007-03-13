## Process this file with automake to produce Makefile.in 

bin_PROGRAMS = ephoto

ephoto_SOURCES = ephoto.c \
		      ephoto_browsing.c \
		      ephoto_database.c \
		      ephoto_edit_view.c \
		      ephoto_exif.c \
		      ephoto_gui.c \
		      ephoto_imaging.c \
		      ephoto_list_view.c \
		      ephoto_main.c \
		      ephoto_nls.c \
		      ephoto_normal_view.c \
		      ephoto_slideshow.c \
		      ephoto_utils.c
		     
ephoto_CFLAGS = @ewl_cflags@ -Wall -Werror

ephoto_LDADD =  @ewl_libs@ -lsqlite3 -lexif

EXTRA_DIST = \
	     ephoto.h
