SUBDIRS = pix

configdir = $(pkgdatadir)/config
config_DATA = \
	definitions \
	actionclasses.cfg bindings.cfg matches.cfg menus.cfg \
	e16keyedit.db \
	fonts.xft.cfg fonts.pango.cfg

menudir = $(pkgdatadir)/config/menus
menu_DATA = \
	desktop.menu enlightenment.menu maintenance.menu settings.menu \
	winops.menu winops_groups.menu winops_layer.menu \
	winops_misc.menu winops_opacity.menu winops_size.menu

EXTRA_DIST = $(config_DATA) $(menu_DATA) strings.c
