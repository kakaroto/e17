#ifndef ICONS_H
#define ICONS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "config.h"

#define PIXMAPDIR "/usr/share/pixmaps"
#define ICONDIR "/usr/share/icons"
#define CRYSTALSVGDIR "/usr/share/icons/crystalsvg"

#define APPLICATIONICON PACKAGE_DATA_DIR"/icons/package_applications.png"
#define COREICON PACKAGE_DATA_DIR"/icons/package_applications.png"
#define EDITORICON PACKAGE_DATA_DIR"/icons/package_editors.png"
#define EDUTAINMENTICON PACKAGE_DATA_DIR"/icons/package_edutainment.png"
#define GAMESICON PACKAGE_DATA_DIR"/icons/package_games.png"
#define GRAPHICSICON PACKAGE_DATA_DIR"/icons/package_graphics.png"
#define INTERNETICON PACKAGE_DATA_DIR"/icons/package_network.png"
#define MULTIMEDIAICON PACKAGE_DATA_DIR"/icons/package_multimedia.png"
#define OFFICEICON PACKAGE_DATA_DIR"/icons/package_wordprocessing.png"
#define PROGRAMMINGICON PACKAGE_DATA_DIR"/icons/package_development.png"
#define SETTINGSICON PACKAGE_DATA_DIR"/icons/package_settings.png"
#define SYSTEMICON PACKAGE_DATA_DIR"/icons/package_system.png"
#define TOYSICON PACKAGE_DATA_DIR"/icons/package_toys.png"
#define UTILITYICON PACKAGE_DATA_DIR"/icons/package_utilities.png"

/* Function Prototypes */
char *set_icon(char *token);
char *find_icon(char *icon);
char *find_fdo_icon(char *icon);

#endif
