MAINTAINERCLEANFILES = Makefile.in default.edj

EDJ = default.edj


EDJE_FLAGS = -v

default.edj:  default.edc main_menu.edc utils.edc slideshow.edc list_left.edc
	$(EDJE_CC) $(EDJE_FLAGS) default.edc -id ../images/

icondir = $(pkgdatadir)/themes
icon_DATA =  $(EDJ)

clean-local:
	rm -rf *.edj *~

uninstall:
	rm -rf $(DESTDIR)$(pkgdatadir)/themes

