SUBDIRS = pix backup

configdata_DATA = \
actionclasses.cfg \
backup-borders.cfg \
backup-colormodifiers.cfg \
backup-cursors.cfg \
backup-imageclasses.cfg \
backup-textclasses.cfg \
backup-tooltips.cfg \
definitions \
control.cfg \
icondefs.cfg \
keybindings.cfg \
menus.cfg \
keybindings.gmc.cfg \
keybindings.nogmc.cfg \
control.kde.cfg \
control.nokde.cfg \
control.rh.cfg \
keybindings.rh.cfg \
menus.rh.cfg

if FSSTD
configdatadir = $(prefix)/share/enlightenment/config
else
configdatadir = $(prefix)/enlightenment/config
endif

EXTRA_DIST = $(configdata_DATA)
