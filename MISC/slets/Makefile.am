AUTOMAKE_OPTIONS = 1.4 foreign

IMAGES = img/bomb.png img/clock.png img/flower.png img/fog3.png img/log.png \
img/sky.png img/book.png img/dynamite.png img/fog1.png img/fog4.png \
img/printer.png img/spider.png img/cdrom.png img/floppy.png \
img/fog2.png img/gail.png img/rows.png

FONTS = fnt/cinema.ttf fnt/notepad.ttf

SOUNDS = snd/menu.wav snd/slethe.wav snd/slot.wav snd/startup.wav

EXTRA_DIST = $(IMAGES) $(FONTS) $(SOUNDS)

SUBDIRS = src

install-data-hook:
	$(mkinstalldirs) $(DESTDIR)$(pkgdatadir)
	$(mkinstalldirs) $(DESTDIR)$(pkgdatadir)/img
	$(mkinstalldirs) $(DESTDIR)$(pkgdatadir)/fnt
	$(mkinstalldirs) $(DESTDIR)$(pkgdatadir)/snd
	for i in $(IMAGES); do $(INSTALL_DATA) $$i $(DESTDIR)$(pkgdatadir)/img ; done
	for i in $(FONTS); do $(INSTALL_DATA) $$i $(DESTDIR)$(pkgdatadir)/fnt ; done
	for i in $(SOUNDS); do $(INSTALL_DATA) $$i $(DESTDIR)$(pkgdatadir)/snd ; done
