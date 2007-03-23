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
		     
ephoto_CFLAGS = @EWL_CFLAGS@ @SQLITE_CFLAGS@ @EXIF_CFLAGS@ -Wall -Werror

ephoto_LDADD =  @EWL_LIBS@ @SQLITE_LIBS@ @EXIF_LIBS@

EXTRA_DIST = \
	     ephoto.h
