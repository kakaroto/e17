AUTOMAKE_OPTIONS = foreign

bin_PROGRAMS = ee2

ee2_SOURCES = map.c file.c file.h cache.c webcam.c about.c browser.c menus.c ee2.c ee2.h

CFLAGS := $(CPPFLAGS) $(CFLAGS) `gtk-config --cflags` 

ee2_LDFLAGS = `gtk-config --libs`

IMAGES = ee2.png ee2-alpha.png
EXTRA_DIST = $(IMAGES)

install-data-hook:
	$(mkinstalldirs) $(DESTDIR)$(pkgdatadir)
	for i in $(IMAGES); do $(INSTALL_DATA) $$i $(DESTDIR)$(pkgdatadir) ; done
