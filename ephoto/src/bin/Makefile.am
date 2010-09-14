## Process this file with automake to produce Makefile.in 

MAINTAINERCLEANFILES = *.o ephoto config.h.in config.h Makefile.in Makefile \
		       stamp-h1

bin_PROGRAMS = ephoto

ephoto_SOURCES = \
		 ephoto.c \
		 ephoto.h \
		 ephoto_config.c \
		 ephoto_flow_browser.c \
		 ephoto_main.c \
		 ephoto_slideshow.c \
		 ephoto_thumb_browser.c

ephoto_CFLAGS = @EET_CFLAGS@ @EVAS_CFLAGS@ @EDJE_CFLAGS@ @EINA_CFLAGS@ @EFREET_MIME_CFLAGS@ @ETHUMB_CFLAGS@ @ELEMENTARY_CFLAGS@ @EIO_CFLAGS@ -Wall -g
ephoto_LDADD = @EET_LIBS@ @EVAS_LIBS@ @EDJE_LIBS@ @EINA_LIBS@ @EFREET_MIME_LIBS@ @ETHUMB_LIBS@ @ELEMENTARY_LIBS@ @EIO_LIBS@

if BUILD_LIBEXIF
ephoto_CFLAGS += @EXIF_CFLAGS@
endif

if BUILD_LIBEXIF
ephoto_LDADD += @EXIF_LIBS@
endif
