
MAINTAINERCLEANFILES = Makefile.in

AM_CFLAGS = \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
-Wall -g @ENKI_CFLAGS@

bin_PROGRAMS = enki

enki_SOURCES = \
								main.c \
								pm_cb.c \
								pm_win.c \
								list_left.c \
								map.c \
								list_photo.c \
								slideshow.c \
								panel_image.c \
								panel_geocaching.c \
								evas_object/photo_object.c \
								evas_object/photos_list_object.c \
								evas_object/tabpanel.c \
								menu.c \
								import.c \
								album_menu.c \
								photo_menu.c \
								collection_menu.c \
								tag_menu.c \
								inwin.c \
								flickr_menu.c \
								download.c \
								main_menu.c

enki_LDADD = @ENKI_LIBS@
