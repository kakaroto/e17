#include <dirent.h>
#include <string.h>             //string funcs
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <libgen.h>

#include <Ecore.h>

#include "global.h"
#include "fdo_paths.h"

#define E_FN_DEL(_fn, _h) if (_h) { _fn(_h); _h = NULL; }
#define E_REALLOC(p, s, n) p = (s *)realloc(p, sizeof(s) * n)
#define E_NEW(s, n) (s *)calloc(n, sizeof(s))
#define E_NEW_BIG(s, n) (s *)malloc(n * sizeof(s))
#define E_FREE(p) { if (p) {free(p); p = NULL;} }

static Fdo_Path_List *_fdo_paths_list_add(Fdo_Path_List * list, char *element);
static int _fdo_paths_list_exist(Fdo_Path_List * list, char *element);
static Fdo_Path_List *_fdo_paths_paths_to_list(char *paths);
static void _fdo_paths_list_del(Fdo_Path_List * list);

static void _fdo_paths_massage_path(char *path, char *home, char *first,
                                    char *second);
static void _fdo_paths_check_and_add(Fdo_Path_List * paths, char *path);
static Fdo_Path_List *_fdo_paths_get(char *env, char *type, char *gnome_extra,
                                     char *kde);
static char *_fdo_paths_recursive_search(char *path, char *d);

/*  We need -
config file full of paths
menus=pathlist
desktops=pathlist
directories=pathlist
icons=pathlist
*/

/* Just a quick and dirty list implemtation that will likely get replaced by 
 * something much saner at a later date.  I wrote most of this while falling
 * asleep.  It will probably scare me when I wake up.  B-)
 *
 * Devilhorns said to make it portable, so we con't rely on any external list 
 * implementation.  So this list is designed specifically for this task.
 *
 * The lists will be tiny.
 * They only store strings.
 * There is no insertion or deletion, only append.
 * Append order must be maintained.
 * The lists will only ever be accessed sequentially, from begining to end.
 *
 * No duplicates in the list,
 *    This is the nasty part of this list implementation.
 *    Insertions involve a linear search for dupes, most of the 
 *    time there won't be any dupes, so the list is searched in
 *    it's entirety.  The lists will be really small, and only created at 
 *    the begining, so no biggy.
 */
static Fdo_Path_List *
_fdo_paths_list_new(char *buffer)
{
   Fdo_Path_List *list;

   list = E_NEW(Fdo_Path_List, 1);
   if ((list) && (buffer))
     {
        list->buffer = strdup(buffer);
        list->length = strlen(list->buffer) + 1;
     }
   return list;
}

static Fdo_Path_List *
_fdo_paths_list_add(Fdo_Path_List * list, char *element)
{
   list->list = (char **)realloc(list->list, (list->size + 1) * sizeof(char *));
   list->list[list->size++] = element;
   return list;
}

static Fdo_Path_List *
_fdo_paths_list_extend(Fdo_Path_List * list, char *element)
{
   int i, length;

   /* Normalize the pointers. */
   for (i = 0; i < list->size; i++)
      list->list[i] -= (long)list->buffer;      /* FIXME: find something better to cast this to. */

   length = strlen(element);
   list->buffer =
      (char *)realloc(list->buffer,
                      (list->length + length + 1 + 1) * sizeof(char));
   memcpy(&(list->buffer[list->length + 1]), element, length + 1);

   /* Restore them. */
   for (i = 0; i < list->size; i++)
      list->list[i] += (long)list->buffer;      /* FIXME: find something better to cast this to. */

   list = _fdo_paths_list_add(list, &(list->buffer[list->length + 1]));
   list->length += length + 1;

   return list;
}

static int
_fdo_paths_list_exist(Fdo_Path_List * list, char *element)
{
   int exist = 0;
   int i;

   for (i = 0; i < list->size; i++)
     {
        if (strcmp(list->list[i], element) == 0)
          {
             exist = 1;
             break;
          }
     }
   return exist;
}

static void
_fdo_paths_list_del(Fdo_Path_List * list)
{
   E_FREE(list->buffer);
   E_FREE(list->list);
   E_FREE(list);
}

static Fdo_Path_List *
_fdo_paths_paths_to_list(char *paths)
{
   Fdo_Path_List *list = NULL;

   list = _fdo_paths_list_new(paths);
   if ((list) && (list->buffer))
     {
        char *start, *end;
        int finished = 0;

        end = list->buffer;
        while (!finished)
          {
             start = end;
             while ((*end != ':') && (*end != '\0'))
                end++;
             if (*end == '\0')
                finished = 1;
             *end = '\0';
             if (!_fdo_paths_list_exist(list, start))
                list = _fdo_paths_list_add(list, start);
             end++;
          }
     }
   return list;
}

static void
_fdo_paths_massage_path(char *path, char *home, char *first, char *second)
{
   int last;

   /* Strip traling slash of first. */
   last = strlen(first) - 1;
   if ((last >= 0) && (first[last] == '/'))
      first[last] = '\0';

   /* Strip traling slash of second. */
   last = strlen(second) - 1;
   if ((last >= 0) && (second[last] == '/'))
      second[last] = '\0';

   if (first[0] == '~')
      sprintf(path, "%s/%s/%s/",
              home, &first[1], &second[(second[0] == '/') ? 1 : 0]);
   else
      sprintf(path, "%s/%s/", first, &second[(second[0] == '/') ? 1 : 0]);
}

static void
_fdo_paths_check_and_add(Fdo_Path_List * paths, char *path)
{
   printf("CHECKING %s", path);
   if (!_fdo_paths_list_exist(paths, path))
     {
        struct stat path_stat;

        /* Check if the path exists. */
        if ((stat(path, &path_stat) == 0) && (S_ISDIR(path_stat.st_mode)))
          {
             printf(" OK");
             _fdo_paths_list_extend(paths, path);
          }
     }
   printf("\n");
}

static Fdo_Path_List *
_fdo_paths_get(char *env, char *type, char *gnome_extra, char *kde)
{
   char *home;
   Fdo_Path_List *paths = NULL;
   Fdo_Path_List *types;
   Fdo_Path_List *gnome_extras;
   Fdo_Path_List *kdes;

   types = _fdo_paths_paths_to_list(type);
   gnome_extras = _fdo_paths_paths_to_list(gnome_extra);
   kdes = _fdo_paths_paths_to_list(kde);

   home = get_home();
   paths = _fdo_paths_list_new(NULL);
   if (paths)
     {
        int i, j;
        char path[MAX_PATH];
        Fdo_Path_List *env_list;

        if (env)
          {
             char *value;

             value = getenv(env);
             printf("ENV %s\n", value);
             env_list = _fdo_paths_paths_to_list(value);
             if (env_list)
               {
                  for (i = 0; i < env_list->size; i++)
                    {
                       for (j = 0; j < types->size; j++)
                         {
                            _fdo_paths_massage_path(path, home,
                                                    env_list->list[i],
                                                    types->list[j]);
                            _fdo_paths_check_and_add(paths, path);
                         }
                    }
               }
          }
     }

   /* Don't sort them, as they are in preferred order from each source. */
   /* Merge the results, there are probably some duplicates. */
   /*
    * Get the pathlist from the config file - type=pathlist
    *   for each path in config
    *      if it is already in paths, skip it
    *      if it exists, add it to end of paths
    * gnome-config --data
    *   for each path in gnome
    *      for each path in type
    *         add type to end of env
    *         if it is already in paths, skip it
    *         if it exists, add it to end of paths
    *      for each path in gnome_extra
    *         add gnome_extra to end of env
    *         if it is already in paths, skip it
    *         if it exists, add it to end of paths
    * for each kde
    *    kde-config --path kde
    *       for each path in kde
    *          if it is already in paths, skip it
    *          if it exists, add it to end of paths
    */

   E_FREE(home);
   E_FN_DEL(_fdo_paths_list_del, kdes);
   E_FN_DEL(_fdo_paths_list_del, gnome_extras);
   E_FN_DEL(_fdo_paths_list_del, types);

   return paths;
}

void
fdo_paths_init()
{
   if (!fdo_paths_menus)
      fdo_paths_menus =
         _fdo_paths_get("XDG_CONFIG_DIRS", "menus", NULL, "xdgconf-menu");
   if (!fdo_paths_directories)
      fdo_paths_directories =
         _fdo_paths_get("XDG_DATA_DIRS", "desktop-directories",
                        "gnome/vfolders", "xdgdata-dirs");
   if (!fdo_paths_desktops)
      fdo_paths_desktops =
         _fdo_paths_get("XDG_DATA_DIRS", "applications",
                        "dist/desktop-files:dist/short-menu:gnome/apps",
                        "xdgdata-apps:apps");
   if (!fdo_paths_icons)
     {
        char *gnome;

        /* FIXME: add ~/.icons to beginning. */
        fdo_paths_icons =
           _fdo_paths_get("XDG_DATA_DIRS", "icons", "dist/icons",
                          "icon:pixmap");
        _fdo_paths_check_and_add(fdo_paths_icons, "/usr/share/pixmaps/");
        gnome = getenv("$GNOME_ICON_PATH");
        if (gnome)
           _fdo_paths_check_and_add(fdo_paths_icons, gnome);
     }
}

void
fdo_paths_shutdown()
{
   E_FN_DEL(_fdo_paths_list_del, fdo_paths_menus);
   E_FN_DEL(_fdo_paths_list_del, fdo_paths_directories);
   E_FN_DEL(_fdo_paths_list_del, fdo_paths_desktops);
   E_FN_DEL(_fdo_paths_list_del, fdo_paths_icons);
}

char *
fdo_paths_search_for_file(Fdo_Paths_Type type, char *file)
{
   int i;
   char *path = NULL;
   char temp[MAX_PATH];
   struct stat path_stat;
   Fdo_Path_List *paths = NULL;

   switch (type)
     {
     case FDO_PATHS_TYPE_MENU:
        paths = fdo_paths_menus;
        break;
     case FDO_PATHS_TYPE_DIRECTORY:
        paths = fdo_paths_directories;
        break;
     case FDO_PATHS_TYPE_DESKTOP:
        paths = fdo_paths_desktops;
        break;
     case FDO_PATHS_TYPE_ICON:
        paths = fdo_paths_icons;
        break;
     }

   for (i = 0; i < paths->size; i++)
     {
        sprintf(temp, "%s%s", paths->list[i], file);
        if (stat(temp, &path_stat) == 0)
           path = strdup(temp);
        else
           path = _fdo_paths_recursive_search(paths->list[i], file);
        if (path)
           break;
     }

   return path;
}

static char *
_fdo_paths_recursive_search(char *path, char *file)
{
   char *fpath = NULL;
   DIR *dir = NULL;

   dir = opendir(path);

   printf("SEARCHING %s FOR %s\n", path, file);
   if (dir != NULL)
     {
        struct dirent *script;

        while ((script = readdir(dir)) != NULL)
          {
             struct stat script_stat;
             char info_text[4096];

             sprintf(info_text, "%s%s", path, script->d_name);
             if ((stat(info_text, &script_stat) == 0))
               {
                  if (S_ISDIR(script_stat.st_mode))
                    {
                       if ((strcmp(basename(info_text), ".") != 0)
                           && (strcmp(basename(info_text), "..") != 0))
                         {
                            sprintf(info_text, "%s%s/", path, script->d_name);
                            fpath =
                               _fdo_paths_recursive_search(info_text, file);
                         }
                    }
                  else
                    {
                       if (strcmp(basename(info_text), file) == 0)
                          fpath = strdup(info_text);
                    }
                  if (fpath)
                     break;
               }
          }
        closedir(dir);
     }

   return fpath;
}
