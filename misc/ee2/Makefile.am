AUTOMAKE_OPTIONS = foreign

bin_PROGRAMS = ee2

ee2_SOURCES = redm_win.c grm_win.c blm_win.c am_win.c mwin.c fx_win.c map.c file.c file.h cache.c webcam.c about.c browser.c ee2.c ee2.h

IMAGES = ee2.png ee2-alpha.png
EXTRA_DIST = $(IMAGES)

install-data-hook:
	$(mkinstalldirs) $(DESTDIR)$(pkgdatadir)
	for i in $(IMAGES); do $(INSTALL_DATA) $$i $(DESTDIR)$(pkgdatadir) ; done
