#include "global.h"
#include "config.h"
#include "icons.h"

char *set_icon(char *token)
{
#ifdef DEBUG
   fprintf(stderr, "Setting Icon: %s\n", token);
#endif
   if (strstr(token, "Core") != NULL) return COREICON;
   if (strstr(token, "Development") != NULL) return PROGRAMMINGICON;
   if (strstr(token, "Editors") != NULL) return EDITORICON;
   if (strstr(token, "Edutainment") != NULL) return EDUTAINMENTICON;
   if (strstr(token, "Games") != NULL) return GAMESICON;
   if (strstr(token, "Graphics") != NULL) return GRAPHICSICON;
   if (strstr(token, "Internet") != NULL)	return INTERNETICON;
   if (strstr(token, "Office") != NULL) return OFFICEICON;
   if (strstr(token, "Programming") != NULL) return PROGRAMMINGICON;
   if (strstr(token, "Toys") != NULL) return TOYSICON;
   if (strstr(token, "Utilities") != NULL) return UTILITYICON;
   if ((strstr(token, "Accessories") != NULL) ||
       (strstr(token, "Applications") != NULL))
     return APPLICATIONICON;
   if ((strstr(token, "Multimedia") != NULL) ||
       (strstr(token, "Sound_Video") != NULL))
     return MULTIMEDIAICON;
   if ((strstr(token, "Preferences") != NULL) ||
       (strstr(token, "Settings") != NULL))
     return SETTINGSICON;
   if ((strstr(token, "System") != NULL) ||
       (strstr(token, "System_Tools") != NULL))
     return SYSTEMICON;
   return token;
}

char *find_icon(char *icon)
{
   char icn[MAX_PATH], path[MAX_PATH];
   char *dir, *icon_size, *icon_theme, *home;

   if (icon == NULL) return DEFAULTICON;

   home = get_home();

   snprintf(icn, sizeof(icn), "%s", icon);
#ifdef DEBUG
   fprintf(stderr, "\tTrying To Find Icon %s\n", icn);
#endif

   /* Check For Unsupported Extension */
   if ((!strcmp(icon + strlen(icon) - 4, ".svg")) || (!strcmp(icon + strlen(icon) - 4, ".ico")))
	return DEFAULTICON;

    /* Check For An Extension, Append PNG If Missing */
   if (strrchr(icon, '.') == NULL) snprintf(icn, sizeof(icn), "%s.png", icon);

    /* Check If Dir Supplied In Desktop File */
   dir = ecore_file_get_dir(icn);
   if (!strcmp(dir, icn) == 0)
     {
	snprintf(path, MAX_PATH, "%s", icn);
	/* Check Supplied Dir For Icon */
	if (ecore_file_exists(path)) return strdup(icn);
     }

   snprintf(path, MAX_PATH, PIXMAPDIR"/%s", icn);
   if (ecore_file_exists(path)) return strdup(path);

   /* Get Icon Options */
   icon_size = get_icon_size();
   icon_theme = get_icon_theme();

   /* Check User Supplied Icon Theme */
   if (icon_theme != NULL)
     {
	fprintf(stderr, "\tUsing Icon Theme: %s\n", icon_theme);
	snprintf(path, MAX_PATH, "%s/%s/apps/%s", icon_theme, icon_size, icn);
	if (ecore_file_exists(path)) return strdup(path);
	snprintf(path, MAX_PATH, "%s/%s/devices/%s", icon_theme, icon_size, icn);
	if (ecore_file_exists(path)) return strdup(path);
	snprintf(path, MAX_PATH, "%s/%s/filesystems/%s", icon_theme, icon_size, icn);
	if (ecore_file_exists(path)) return strdup(path);
     }

   snprintf(path, MAX_PATH, CRYSTALSVGDIR"/%s/apps/%s", icon_size, icn);
   if (ecore_file_exists(path)) return strdup(path);
   snprintf(path, MAX_PATH, CRYSTALSVGDIR"/%s/devices/%s", icon_size, icn);
   if (ecore_file_exists(path)) return strdup(path);
   snprintf(path, MAX_PATH, CRYSTALSVGDIR"/%s/filesystems/%s", icon_size, icn);
   if (ecore_file_exists(path)) return strdup(path);

    /* We Did Not Find the icon in theme dir,
		check default theme before setting a default icon */
   snprintf(path, MAX_PATH, ICONDIR"/hicolor/%s/apps/%s", icon_size, icn);
   if (ecore_file_exists(path)) return strdup(path);
   snprintf(path, MAX_PATH, ICONDIR"/hicolor/%s/devices/%s", icon_size, icn);
   if (ecore_file_exists(path)) return strdup(path);
   snprintf(path, MAX_PATH, ICONDIR"/hicolor/%s/filesystems/%s", icon_size, icn);
   if (ecore_file_exists(path)) return strdup(path);

   return DEFAULTICON;
}
