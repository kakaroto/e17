MAINTAINERCLEANFILES = Makefile.in default.edj

EDJE_CC = @edje_cc@
EDJ = default.edj
EDJE_FLAGS = -v

default.edj:  default.edc \
libraries.edc \
utils.edc \
slideshow.edc \
list_left.edc \
list_left_menu.edc \
main_panel.edc \
map.edc main.edc \
main_page.edc \
geocaching.edc \
geocaching_bubble.edc \
photo.edc \
photo_library.edc \
photo_simple.edc \
photo_import.edc \
photo_slideshow.edc \
icons.edc \
flickr.edc \
minislideshow.edc \
elm.edc \
album_header.edc \
panel_image.edc \
win_album_new.edc \
win_album_rename.edc \
win_album_delete.edc \
win_preferences.edc \
win_album_tag_new.edc \
win_album_collection_new.edc \
win_photo_delete.edc\
win_library_delete.edc \
photo_menu_parts.edc 
	$(EDJE_CC) $(EDJE_FLAGS) default.edc -id ../images/

icondir = $(pkgdatadir)/themes
icon_DATA =  $(EDJ)

clean-local:
	rm -rf *.edj *~

uninstall:
	rm -rf $(DESTDIR)$(pkgdatadir)/themes

