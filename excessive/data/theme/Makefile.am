MAINTAINERCLEANFILES = Makefile.in theme.edj

EDJE_CC = @edje_cc@
EDJ = theme.edj
EDJE_FLAGS = -v

theme.edj:  theme.edc ../images/triangle.png ../images/triangle_hi.png ../images/background.jpg
	$(EDJE_CC) $(EDJE_FLAGS) theme.edc -id ../images/ theme.edj

icondir = $(pkgdatadir)/themes
icon_DATA =  $(EDJ)

clean-local:
	rm -rf *.edj *~

uninstall:
	rm -rf $(DESTDIR)$(pkgdatadir)/themes

