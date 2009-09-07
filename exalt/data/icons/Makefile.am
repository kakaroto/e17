MAINTAINERCLEANFILES = Makefile.in

EDJE_CC = @EDJE_CC@
EDJE_FLAGS = -v

filesdir =  $(pkgdatadir)/icons
files_DATA = theme.edj

EXTRA_DIST = \
theme.edc \
encryption.png \
ethernet.png \
ethernet_not_activate_link.png \
ethernet_not_activate_link_small.png \
ethernet.png \
ethernet_small.png \
logo.png \
quality_background.png \
quality_drag.png \
wireless_not_activate_link.png \
wireless_not_activate_link_small.png \
wireless.png \
wireless_small.png

theme.edj: Makefile.am $(EXTRA_DIST)
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/icons/theme.edc \
	$(top_builddir)/data/icons/theme.edj

clean-local:
	 rm -rf *.edj *~

uninstall:
	rm -rf $(DESTDIR)$(pkgdatadir)
