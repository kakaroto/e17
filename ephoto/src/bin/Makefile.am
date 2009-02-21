## Process this file with automake to produce Makefile.in 

MAINTAINERCLEANFILES = *.o ephoto config.h.in config.h Makefile.in Makefile \
		       stamp-h1

bin_PROGRAMS = ephoto

ephoto_SOURCES = \
		 ephoto.c \
		 ephoto_exif.c \
		 ephoto_fsystem.c \
		 ephoto_global.c \
		 ephoto_imaging.c \
		 ephoto_main.c \
		 ephoto_normal_view.c \
		 ephoto_single_view.c \
		 ephoto_utils.c \
		 ephoto.h \
		 ephoto_exif.h \
		 ephoto_fsystem.h \
		 ephoto_imaging.h \
		 ephoto_utils.h

ephoto_CFLAGS= @EWL_CFLAGS@ @EXIF_CFLAGS@ @EFREET_MIME_CFLAGS@ @EPSILON_CFLAGS@ -Wall -g
ephoto_LDADD =  @EWL_LIBS@ @EXIF_LIBS@ @EFREET_MIME_LIBS@ @EPSILON_LIBS@
