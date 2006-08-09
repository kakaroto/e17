#include "config.h"
#include "global.h"
#include "category.h"
#include "fdo_icons.h"
#include "fdo_desktops.h"

extern int reject_count, not_over_count;

static void _fdo_desktops_desktop_del(Fdo_Desktop * desktop);

static Ecore_Hash *ini_file_cache;
static Ecore_Hash *desktop_cache;


Ecore_Hash *
fdo_desktops_parse_ini_file(char *file)
{
   Ecore_Hash *result;

/* FIXME: should probably look in ini_file_cache first. */
   result = ecore_hash_new(ecore_str_hash, ecore_str_compare);
   if (result)
     {
        FILE *f;
        char buffer[MAX_PATH];
        Ecore_Hash *current = NULL;

        f = fopen(file, "r");
        if (!f)
          {
             fprintf(stderr, "ERROR: Cannot Open File %s\n", file);
             ecore_hash_destroy(result);
             return NULL;
          }
        ecore_hash_set_free_key(result, free);
        ecore_hash_set_free_value(result, (Ecore_Free_Cb) ecore_hash_destroy);
        *buffer = '\0';
#ifdef DEBUG
        fprintf(stdout, "PARSING INI %s\n", file);
#endif
        while (fgets(buffer, sizeof(buffer), f) != NULL)
          {
             char *c;
             char *key;
             char *value;

             c = buffer;
             /* Strip preceeding blanks. */
             while (((*c == ' ') || (*c == '\t')) && (*c != '\n') && (*c != '\0'))
                c++;
             /* Skip blank lines and comments */
             if ((*c == '\0') || (*c == '\n') || (*c == '#'))
                continue;
             if (*c == '[')     /* New group. */
               {
                  key = c + 1;
                  while ((*c != ']') && (*c != '\n') && (*c != '\0'))
                     c++;
                  *c++ = '\0';
                  current = ecore_hash_new(ecore_str_hash, ecore_str_compare);
                  if (current)
                    {
                       ecore_hash_set_free_key(current, free);
                       ecore_hash_set_free_value(current, free);
                       ecore_hash_set(result, strdup(key), current);
#ifdef DEBUG
                       fprintf(stdout, "  GROUP [%s]\n", key);
#endif
                    }
               }
             else if (current)  /* key=value pair of current group. */
               {
                  key = c;
                  /* Find trailing blanks or =. */
                  while ((*c != '=') && (*c != ' ') && (*c != '\t') && (*c != '\n') && (*c != '\0'))
                     c++;
                  if (*c != '=')        /* Find equals. */
                    {
                       *c++ = '\0';
                       while ((*c != '=') && (*c != '\n') && (*c != '\0'))
                          c++;
                    }
                  if (*c == '=')        /* Equals found. */
                    {
                       *c++ = '\0';
                       /* Strip preceeding blanks. */
                       while (((*c == ' ') || (*c == '\t')) && (*c != '\n') && (*c != '\0'))
                          c++;
                       value = c;
                       /* Find end. */
                       while ((*c != '\n') && (*c != '\0'))
                          c++;
                       *c++ = '\0';
                       /* FIXME: should strip space at end, then unescape value. */
                       ecore_hash_set(current, strdup(key), strdup(value));
#ifdef DEBUG
                       fprintf(stdout, "    %s=%s\n", key, value);
#endif
                    }
               }

          }
        buffer[0] = (char)0;

        fclose(f);
        ecore_hash_set(ini_file_cache, strdup(file), result);
     }
   return result;
}


Fdo_Desktop *
fdo_desktops_parse_desktop_file(char *file)
{
   Fdo_Desktop *result;

   result = (Fdo_Desktop *) ecore_hash_get(desktop_cache, file);
   if (!result)
     {
        result = calloc(1, sizeof(Fdo_Desktop));
        if (result)
          {
             result->data = fdo_desktops_parse_ini_file(file);
             if (result->data)
               {
                  result->group = (Ecore_Hash *) ecore_hash_get(result->data, "Desktop Entry");
                  if (!result->group)
                     result->group = (Ecore_Hash *) ecore_hash_get(result->data, "KDE Desktop Entry");
                  if (result->group)
                    {
                       char *value;

                       value = (char *)ecore_hash_get(result->group, "Name");
                       if (value)
                          result->name = strdup(value);
                       value = (char *)ecore_hash_get(result->group, "GenericName");
                       if (value)
                          result->generic = strdup(value);
                       value = (char *)ecore_hash_get(result->group, "Comment");
                       if (value)
                          result->comment = strdup(value);
                       value = (char *)ecore_hash_get(result->group, "Type");
                       if (value)
                          result->type = strdup(value);
                       value = (char *)ecore_hash_get(result->group, "Categories");
                       if (value)
		       {
                          result->categories = strdup(value);
                          result->Categories = dumb_tree_from_paths(value);
		       }
                       value = (char *)ecore_hash_get(result->group, "OnlyShowIn");
                       if (value)
                          result->OnlyShowIn = dumb_tree_from_paths(value);
                       value = (char *)ecore_hash_get(result->group, "NotShowIn");
                       if (value)
                          result->NotShowIn = dumb_tree_from_paths(value);
                       value = (char *)ecore_hash_get(result->group, "Exec");
                       if (value)
                          result->exec = strdup(value);
                       value = (char *)ecore_hash_get(result->group, "Icon");
                       if (value)
               	       {
                           result->icon = strdup(value);
                   	   result->icon_path = find_icon(result->icon);
               	       }
                       value = (char *)ecore_hash_get(result->group, "X-KDE-StartupNotify");
                       if (value)
                          result->startup = (!strcmp(value, "true")) ? "1" : "0";
                       value = (char *)ecore_hash_get(result->group, "StartupNotify");
                       if (value)
                          result->startup = (!strcmp(value, "true")) ? "1" : "0";
                       value = (char *)ecore_hash_get(result->group, "StartupWMClass");
                       if (value)
                          result->window_class = strdup(value);
                    }

                  ecore_hash_set(desktop_cache, strdup(file), result);
               }
             else
               {
                  free(result);
                  result = NULL;
               }
          }
     }
   return result;
}


void
fdo_desktops_init()
{
   if (!ini_file_cache)
     {
        ini_file_cache = ecore_hash_new(ecore_str_hash, ecore_str_compare);
        if (ini_file_cache)
          {
             ecore_hash_set_free_key(ini_file_cache, free);
             ecore_hash_set_free_value(ini_file_cache, (Ecore_Free_Cb) ecore_hash_destroy);
          }
     }
   if (!desktop_cache)
     {
        desktop_cache = ecore_hash_new(ecore_str_hash, ecore_str_compare);
        if (desktop_cache)
          {
             ecore_hash_set_free_key(desktop_cache, free);
             ecore_hash_set_free_value(desktop_cache, (Ecore_Free_Cb) _fdo_desktops_desktop_del);
          }
     }
}


void
fdo_desktops_shutdown()
{
   if (ini_file_cache)
     {
        ecore_hash_destroy(ini_file_cache);
        ini_file_cache = NULL;
     }
   if (desktop_cache)
     {
        ecore_hash_destroy(desktop_cache);
        desktop_cache = NULL;
     }
}


static void
_fdo_desktops_desktop_del(Fdo_Desktop * desktop)
{
   if (desktop->NotShowIn)
      dumb_tree_del(desktop->NotShowIn);
   if (desktop->OnlyShowIn)
      dumb_tree_del(desktop->OnlyShowIn);
   if (desktop->Categories)
      dumb_tree_del(desktop->Categories);
   free(desktop);
}



/*
main.c
   void fdo_desktops_init()
   void fdo_desktops_shutdown()


fdo_paths_desktops = _fdo_paths_get(NULL, "XDG_DATA_HOME", "XDG_DATA_DIRS",
                           "~/.local/share", "/usr/local/share:/usr/share", "applications",
                           "dist/desktop-files:dist/short-menu:gnome/apps", "xdgdata-apps:apps");
_fdo_paths_check_and_add(fdo_paths_desktops, "/usr/share/update-desktop-files/templates");

fdo_paths_icons = _fdo_paths_get("~/.icons", "XDG_DATA_HOME", "XDG_DATA_DIRS", "~/.local/share", "/usr/local/share:/usr/share", "icons",
                          "dist/icons", "icon:pixmap");
_fdo_paths_check_and_add(fdo_paths_icons, "/usr/share/pixmaps/");
gnome = getenv("$GNOME_ICON_PATH");
if (gnome)
   _fdo_paths_check_and_add(fdo_paths_icons, gnome);


fdo_menus.c
   Desktop *desktop = fdo_desktops_parse_desktop_file(app);

menus.c
   void parse_desktop_file(char *app, char *menu_path)
   void parse_debian_file(char *file)


void parse_desktop_file(char *app, char *menu_path)
   Desktop *desktop = fdo_desktops_parse_desktop_file(app);
   eap->icon_path = find_icon(eap->icon);
   void _parse_process_file(app, menu_path, eap);

void parse_debian_file(char *file)
   char *name = _parse_buffer(buffer, "title=");
      //char *d = _parse_get_t(t);
   eap->icon_path = find_icon(eap->icon);
   void _parse_process_file(file, NULL, eap);

Desktop *desktop = fdo_desktops_parse_desktop_file(app);
   result = (Desktop *) ecore_hash_get(desktop_cache, file);
   Ecore_Hash *result->data = fdo_desktops_parse_ini_file(file);
   result->Categories = dumb_tree_from_paths(temp);
   ecore_hash_set(desktop_cache, strdup(file), result);

Ecore_Hash *result->data = fdo_desktops_parse_ini_file(file);
   ecore_hash_set(ini_file_cache, strdup(file), result);

void _parse_process_file(app, menu_path, eap);
   write_icon(path, eap);
   category = _parse_find_category(eap->categories);
   modify_order(order_path, eap->eap_name);

char *find_icon(eap->icon);
   icon_size = get_icon_size();
   icon_theme = get_icon_theme();
   static char *find_fdo_icon(char *icon, char *icon_size, char *icon_theme)
      theme_path = fdo_paths_search_for_file(FDO_PATHS_TYPE_ICON, icn, 1, NULL, NULL);
      Ecore_Hash *result->data = fdo_desktops_parse_ini_file(theme_path);
      directory_paths = dumb_tree_from_paths(directories);
      found = fdo_paths_search_for_file(FDO_PATHS_TYPE_ICON, path, 0, NULL, NULL);

void fdo_desktops_init()
   ini_file_cache = ecore_hash_new(ecore_str_hash, ecore_str_compare);
   desktop_cache = ecore_hash_new(ecore_str_hash, ecore_str_compare);
   ecore_hash_set_free_value(desktop_cache, (Ecore_Free_Cb) _fdo_desktops_desktop_del);

void fdo_desktops_shutdown()
   ecore_hash_destroy(ini_file_cache);
   ecore_hash_destroy(desktop_cache);

static void _fdo_desktops_desktop_del(Desktop * desktop)
   dumb_tree_del(desktop->Categories);

 */
