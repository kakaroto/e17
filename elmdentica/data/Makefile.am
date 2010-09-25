SUBDIRS = themes

MAINTAINERCLEANFILES = Makefile.in

desktopdir = $(datadir)/applications
dist_desktop_DATA = elmdentica.desktop

appicondir = $(datadir)/pixmaps
dist_appicon_DATA = elmdentica.png

ebconfdir = @CONFIGDIR@
ebconf_in_files = elmdentica.conf.in
ebconf_DATA = $(elmdentica_in_files:.in=)

EXTRA_DIST = \
	elmdentica.schemas

#schemasdir = @GCONF_SCHEMA_FILE_DIR@
#schemas_FILE = elmdentica.schemas
#schemas_in_files = $(schemas_FILE).in
#schemas_DATA = $(schemas_in_files:.schemas.in=.schemas)
# 
#@INTLTOOL_SCHEMAS_RULE@
# 
#if GCONF_SCHEMAS_INSTALL
#install-data-local: $(schemas_DATA)
#GCONF_CONFIG_SOURCE=$(GCONF_SCHEMA_CONFIG_SOURCE) $(GCONFTOOL) --makefile-install-rule $(schemas_DATA)
#endif
