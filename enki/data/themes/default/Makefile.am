MAINTAINERCLEANFILES = Makefile.in default.edj

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
photo.edc \
photo_library.edc \
photo_simple.edc \
photo_import.edc \
photo_slideshow.edc \
icons.edc \
flickr.edc \
minislideshow.edc 
	$(EDJE_CC) $(EDJE_FLAGS) default.edc -id ../images/

icondir = $(pkgdatadir)/themes
icon_DATA =  $(EDJ)

clean-local:
	rm -rf *.edj *~

uninstall:
	rm -rf $(DESTDIR)$(pkgdatadir)/themes

