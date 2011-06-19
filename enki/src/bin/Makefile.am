
MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
@ENKI_CFLAGS@

bin_PROGRAMS = enki enki-slideshow

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
import.c \
album_menu.c \
photo_menu.c \
collection_menu.c \
tag_menu.c \
inwin.c \
netsync_album.c \
netsync.c \
download.c \
upload.c \
main_menu.c \
evas_object/photos_list_object.c \
evas_object/photo_object.c \
evas_object/slideshow_object.c \
evas_object/tabpanel.c \
evas_object/enki_elm_genlist_tree.c \
evas_object/edje_mempool.c

enki_LDADD = @ENKI_LIBS@ @LTLIBINTL@


enki_slideshow_SOURCES = \
main_slideshow.c  \
slideshow.c
enki_slideshow_LDADD = @ENKI_LIBS@
