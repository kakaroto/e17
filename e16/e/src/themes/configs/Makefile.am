SUBDIRS = pix

configdata_DATA = \
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
