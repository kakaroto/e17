SUBDIRS = pix backup

configdata_DATA = \
backup-borders.cfg \
backup-colormodifiers.cfg \
backup-cursors.cfg \
backup-imageclasses.cfg \
backup-textclasses.cfg \
backup-tooltips.cfg \
definitions \
control.cfg \
keybindings.cfg \
menus.cfg

if FSSTD
configdatadir = $(prefix)/share/enlightenment/config
else
configdatadir = $(prefix)/enlightenment/config
endif

EXTRA_DIST = $(configdata_DATA)
