AUTOMAKE_OPTIONS = foreign

bin_PROGRAMS = ee2

ee2_SOURCES = scale.c bright.c redm_win.c grm_win.c blm_win.c am_win.c mwin.c fx_win.c map.c file.c file.h cache.c webcam.c about.c browser.c ee2.c ee2.h dnd.c dnd.h

IMAGES = ee2.png ee2-alpha.png
ICONS = icons/alpha_mod.xpm icons/blue_mod.xpm icons/brite_mod.xpm \
        icons/cam_mod.xpm   icons/fx_mod.xpm   icons/green_mod.xpm \
	icons/main_mod.xpm  icons/red_mod.xpm  icons/rotate_mod.xpm \
	icons/scale_mod.xpm icons/sgrab_mod.xpm icons/texmap_mod.xpm
EXTRA_DIST = $(IMAGES) $(ICONS) ee2.spec.in ee2.spec

install-data-hook:
	$(mkinstalldirs) $(DESTDIR)$(pkgdatadir)/icons/
	for i in $(IMAGES); do $(INSTALL_DATA) $(srcdir)/$$i $(DESTDIR)$(pkgdatadir) ; done
	for i in $(ICONS); do $(INSTALL_DATA) $(srcdir)/$$i $(DESTDIR)$(pkgdatadir)/icons ; done
