MAINTAINERCLEANFILES = Makefile.in

EDJE_CC = @edje_cc@
EDJE_FLAGS = -v -id images -fd fonts

filesdir = $(datadir)
files_DATA = \
mount.edj \
mount.edc \
fonts/VeraBd.ttf \
images/dev-camera.png \
images/dev-cdrom.png \
images/dev-cdwriter.png \
images/dev-dvd.png \
images/dev-floppy.png \
images/dev-harddisk.png \
images/dev-ipod.png \
images/dev-network.png \
images/dev-psp.png \
images/dev-usb.png \
images/dev-zipdisk.png \
images/e17_ibar_bg_h.png \
images/e17_ibar_bg_v.png \
images/lock.png

EXTRA_DIST = $(files_DATA)

mount.edj: Makefile mount.edc
	 $(EDJE_CC) $(EDJE_FLAGS) \
	 mount.edc \
	 mount.edj

clean-local:
	rm -f *.edj
