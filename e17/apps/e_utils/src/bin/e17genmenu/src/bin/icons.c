#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "global.h"
#include "config.h"
#include "icons.h"

char *
set_icon(char *token)
{
#ifdef DEBUG
   fprintf(stderr, "Setting Icon: %s\n", token);
#endif
   if (!strcmp(token, "Core"))
      return COREICON;
   if (!strcmp(token, "Development"))
      return PROGRAMMINGICON;
   if (!strcmp(token, "Editors"))
      return EDITORICON;
   if (!strcmp(token, "Edutainment"))
      return EDUTAINMENTICON;
   if (!strcmp(token, "Games"))
      return GAMESICON;
   if (!strcmp(token, "Graphics"))
      return GRAPHICSICON;
   if (!strcmp(token, "Internet"))
      return INTERNETICON;
   if (!strcmp(token, "Office"))
      return OFFICEICON;
   if (!strcmp(token, "Programming"))
      return PROGRAMMINGICON;
   if (!strcmp(token, "Toys"))
      return TOYSICON;
   if (!strcmp(token, "Utilities"))
      return UTILITYICON;
   if ((!strcmp(token, "Accessories")) || (!strcmp(token, "Applications")))
      return APPLICATIONICON;
   if ((!strcmp(token, "Multimedia")) || (!strcmp(token, "Sound_Video")))
      return MULTIMEDIAICON;
   if ((!strcmp(token, "Preferences")) || (!strcmp(token, "Settings")))
      return SETTINGSICON;
   if ((!strcmp(token, "System")) || (!strcmp(token, "System_Tools")))
      return SYSTEMICON;

   return DEFAULTICON;
}

char *
find_icon(char *icon)
{
   char icn[PATH_MAX], path[PATH_MAX];
   char *dir, *icon_size, *icon_theme, *home;

   if (icon == NULL)
      return DEFAULTICON;

   home = get_home();

   snprintf(icn, sizeof(icn), "%s", icon);
#ifdef DEBUG
   fprintf(stderr, "\tTrying To Find Icon %s\n", icn);
#endif

   /* Check For Unsupported Extension */
   if ((!strcmp(icon + strlen(icon) - 4, ".svg"))
       || (!strcmp(icon + strlen(icon) - 4, ".ico")))
      return DEFAULTICON;

   /* Check For An Extension, Append PNG If Missing */
   if (strrchr(icon, '.') == NULL)
      snprintf(icn, sizeof(icn), "%s.png", icon);

   /* Check If Dir Supplied In Desktop File */
   dir = ecore_file_get_dir(icn);
   if (!strcmp(dir, icn))
     {
        snprintf(path, sizeof(path), "%s", icn);
        /* Check Supplied Dir For Icon */
        if (ecore_file_exists(path))
           return strdup(icn);
     }

   snprintf(path, sizeof(path), PIXMAPDIR "/%s", icn);
   if (ecore_file_exists(path))
      return strdup(path);

   /* Get Icon Options */
   icon_size = get_icon_size();
   icon_theme = get_icon_theme();

   /* Check User Supplied Icon Theme */
   if (icon_theme != NULL)
     {
        fprintf(stderr, "\tUsing Icon Theme: %s\n", icon_theme);
        snprintf(path, sizeof(path), "%s/%s/apps/%s", icon_theme, icon_size,
                 icn);
        if (ecore_file_exists(path))
           return strdup(path);
        snprintf(path, sizeof(path), "%s/%s/devices/%s", icon_theme, icon_size,
                 icn);
        if (ecore_file_exists(path))
           return strdup(path);
        snprintf(path, sizeof(path), "%s/%s/filesystems/%s", icon_theme,
                 icon_size, icn);
        if (ecore_file_exists(path))
           return strdup(path);
     }

   snprintf(path, sizeof(path), CRYSTALSVGDIR "/%s/apps/%s", icon_size, icn);
   if (ecore_file_exists(path))
      return strdup(path);
   snprintf(path, sizeof(path), CRYSTALSVGDIR "/%s/devices/%s", icon_size, icn);
   if (ecore_file_exists(path))
      return strdup(path);
   snprintf(path, sizeof(path), CRYSTALSVGDIR "/%s/filesystems/%s", icon_size,
            icn);
   if (ecore_file_exists(path))
      return strdup(path);

   /* We Did Not Find the icon in theme dir,
    * check default theme before setting a default icon */
   snprintf(path, sizeof(path), ICONDIR "/hicolor/%s/apps/%s", icon_size, icn);
   if (ecore_file_exists(path))
      return strdup(path);
   snprintf(path, sizeof(path), ICONDIR "/hicolor/%s/devices/%s", icon_size,
            icn);
   if (ecore_file_exists(path))
      return strdup(path);
   snprintf(path, sizeof(path), ICONDIR "/hicolor/%s/filesystems/%s", icon_size,
            icn);
   if (ecore_file_exists(path))
      return strdup(path);

   return DEFAULTICON;
}
