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

struct _config_exe_data
{
   char *home;
   Fdo_Path_List *types;
   int done;
};

static Fdo_Path_List *_fdo_paths_list_new(char *buffer);
static Fdo_Path_List *_fdo_paths_list_add(Fdo_Path_List * list, char *element);
static int _fdo_paths_list_exist(Fdo_Path_List * list, char *element);
static void _fdo_paths_list_del(Fdo_Path_List * list);

static Fdo_Path_List *_fdo_paths_get(char *before, char *env_home, char *env,
                                     char *env_home_default, char *env_default,
                                     char *type, char *gnome_extra, char *kde);
static void _fdo_paths_massage_path(char *path, char *home, char *first,
                                    char *second);
static void _fdo_paths_check_and_add(Fdo_Path_List * paths, char *path);
static void _fdo_paths_exec_config(char *home, Fdo_Path_List * extras,
                                   char *cmd);

static char *_fdo_paths_recursive_search(char *path, char *d,
                                         int (*func) (const void *data,
                                                      char *path),
                                         const void *data);

static int _fdo_paths_cb_exe_exit(void *data, int type, void *event);

/*
 * This conforms with XDG Base Directory Specification version 0.6
 */

void
fdo_paths_init()
{
   if (!fdo_paths_menus)
      fdo_paths_menus =
         _fdo_paths_get(NULL, "XDG_CONFIG_HOME", "XDG_CONFIG_DIRS", "~/.config",
                        "/etc/xdg", "menus", NULL, "xdgconf-menu");
   if (!fdo_paths_directories)
      fdo_paths_directories =
         _fdo_paths_get(NULL, "XDG_DATA_HOME", "XDG_DATA_DIRS",
                        "~/.local/share", "/usr/local/share:/usr/share",
                        "desktop-directories", "gnome/vfolders",
                        "xdgdata-dirs");
   if (!fdo_paths_desktops)
      fdo_paths_desktops =
         _fdo_paths_get(NULL, "XDG_DATA_HOME", "XDG_DATA_DIRS",
                        "~/.local/share", "/usr/local/share:/usr/share",
                        "applications",
                        "dist/desktop-files:dist/short-menu:gnome/apps",
                        "xdgdata-apps:apps");
   if (!fdo_paths_icons)
     {
        char *gnome;

        fdo_paths_icons =
           _fdo_paths_get("~/.icons", "XDG_DATA_HOME", "XDG_DATA_DIRS",
                          "~/.local/share", "/usr/local/share:/usr/share",
                          "icons", "dist/icons", "icon:pixmap");
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

/** Search for a file in fdo compatible locations.
 *
 * This will search through all the diretories of a particular type, looking 
 * for the file.  It will recurse into subdirectories.  If func is NULL, then
 * only the first file found will be returned.  If func is defined, then each
 * file found will be passed to func, until func returns 1.
 *
 * @param   type The type of directories to search.
 * @param   file The file to search for.
 * @param   sub Should we search sub directories.
 * @param   func A function to call for each file found.
 * @param   data A pointer to pass on to func.
 */
char *
fdo_paths_search_for_file(Fdo_Paths_Type type, char *file, int sub,
                          int (*func) (const void *data, char *path),
                          const void *data)
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
          {
             path = strdup(temp);
             if (func)
                if (func(data, path))
                   break;
          }
        else if (sub)
           path = _fdo_paths_recursive_search(paths->list[i], file, func, data);
        if (path && (!func))
           break;
     }

   return path;
}

/** Split a list of paths into a path list.
 *
 * The list of paths can use any one of ;:, to seperate the paths.
 * You can also escape the :;, with \.
 *
 * FIXME: The concept here is still buggy, but it should do for now.
 *
 * @param   paths A list of paths.
 */
Fdo_Path_List *
fdo_paths_paths_to_list(char *paths)
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
	     do   /* FIXME: There is probably a better way to do this. */
	        {
                   while ((*end != ';') && (*end != ':') && (*end != ',') && (*end != '\0'))
                      end++;
	        }
             while ((end != list->buffer) && (*(end - 1) == '\\') && (*end != '\0'));  /* Ignore any escaped ;:, */
	     /* FIXME: We still need to unescape it now. */
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
_fdo_paths_get(char *before, char *env_home, char *env, char *env_home_default,
               char *env_default, char *type, char *gnome_extra, char *kde)
{
   char *home;
   Fdo_Path_List *paths = NULL;
   Fdo_Path_List *types;
   Fdo_Path_List *gnome_extras;
   Fdo_Path_List *kdes;
   Ecore_Event_Handler *exit_handler;

   /* Don't sort them, as they are in preferred order from each source. */
   /* Merge the results, there are probably some duplicates. */

   types = fdo_paths_paths_to_list(type);
   gnome_extras = fdo_paths_paths_to_list(gnome_extra);
   kdes = fdo_paths_paths_to_list(kde);

   home = get_home();
   if (home)
     {
        int last;

        /* Strip traling slash of home. */
        last = strlen(home) - 1;
        if ((last >= 0) && (home[last] == '/'))
           home[last] = '\0';
     }

   paths = _fdo_paths_list_new(NULL);
   if (paths)
     {
        int i, j;
        char path[MAX_PATH];
        Fdo_Path_List *env_list;

        if (before)
          {
             Fdo_Path_List *befores;

             befores = fdo_paths_paths_to_list(before);
             if (befores)
               {
                  for (i = 0; i < befores->size; i++)
                    {
                       _fdo_paths_massage_path(path, home,
                                               befores->list[i], NULL);
                       _fdo_paths_check_and_add(paths, path);
                    }
                  E_FN_DEL(_fdo_paths_list_del, befores);
               }
          }

        if (env_home)
          {
             char *value;

             value = getenv(env_home);
             if ((value == NULL) || (value[0] == '\0'))
                value = env_home_default;
             env_list = fdo_paths_paths_to_list(value);
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
                  E_FN_DEL(_fdo_paths_list_del, env_list);
               }
          }

        if (env)
          {
             char *value;

             value = getenv(env);
             if ((value == NULL) || (value[0] == '\0'))
                value = env_default;
             env_list = fdo_paths_paths_to_list(value);
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
                  E_FN_DEL(_fdo_paths_list_del, env_list);
               }
          }
     }

   /*
    * Get the pathlist from the config file - type=pathlist
    *   for each path in config
    *      if it is already in paths, skip it
    *      if it exists, add it to end of paths
    */

   exit_handler =
      ecore_event_handler_add(ECORE_EXE_EVENT_DEL, _fdo_paths_cb_exe_exit,
                              paths);
   if (exit_handler)
     {
        int i;

        _fdo_paths_exec_config(home, types, "gnome-config --datadir");
        _fdo_paths_exec_config(home, gnome_extras, "gnome-config --datadir");

        for (i = 0; i < kdes->size; i++)
          {
             char cmd[128];

             sprintf(cmd, "kde-config --path %s", kdes->list[i]);
             _fdo_paths_exec_config(home, NULL, cmd);
          }

        ecore_event_handler_del(exit_handler);
     }

   E_FREE(home);
   E_FN_DEL(_fdo_paths_list_del, kdes);
   E_FN_DEL(_fdo_paths_list_del, gnome_extras);
   E_FN_DEL(_fdo_paths_list_del, types);

   return paths;
}

static void
_fdo_paths_massage_path(char *path, char *home, char *first, char *second)
{
   int last;

   /* Strip traling slash of first. */
   last = strlen(first) - 1;
   if ((last >= 0) && (first[last] == '/'))
      first[last] = '\0';

   if (second)
     {
        /* Strip traling slash of second. */
        last = strlen(second) - 1;
        if ((last >= 0) && (second[last] == '/'))
           second[last] = '\0';
     }

   if (second)
     {
        if (first[0] == '~')
           sprintf(path, "%s%s/%s/",
                   home, &first[1], &second[(second[0] == '/') ? 1 : 0]);
        else
           sprintf(path, "%s/%s/", first, &second[(second[0] == '/') ? 1 : 0]);
     }
   else
     {
        if (first[0] == '~')
           sprintf(path, "%s%s/", home, &first[1]);
        else
           sprintf(path, "%s/", first);
     }
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

static void
_fdo_paths_exec_config(char *home, Fdo_Path_List * extras, char *cmd)
{
   Ecore_Exe *exe;
   struct _config_exe_data ced;

   ced.home = home;
   ced.types = extras;
   ced.done = 0;
   exe =
      ecore_exe_pipe_run(cmd,
                         ECORE_EXE_PIPE_AUTO | ECORE_EXE_PIPE_READ |
                         ECORE_EXE_PIPE_READ_LINE_BUFFERED, &ced);
   if (exe)
     {
        ecore_exe_tag_set(exe, "genmenu/fdo");
        while (ced.done == 0)
          {
             ecore_main_loop_iterate();
             usleep(10);
          }
     }
}

static char *
_fdo_paths_recursive_search(char *path, char *file,
                            int (*func) (const void *data, char *path),
                            const void *data)
{
   char *fpath = NULL;
   DIR *dir = NULL;

   dir = opendir(path);

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
                               _fdo_paths_recursive_search(info_text, file,
                                                           func, data);
                         }
                    }
                  else
                    {
                       if (strcmp(basename(info_text), file) == 0)
                         {
                            fpath = strdup(info_text);
                            if (func)
                               if (func(data, path))
                                  break;
                         }
                    }
                  if (fpath && (!func))
                     break;
               }
          }
        closedir(dir);
     }

   return fpath;
}

static int
_fdo_paths_cb_exe_exit(void *data, int type, void *event)
{
   Ecore_Exe_Event_Del *ev;
   Fdo_Path_List *paths;
   Fdo_Path_List *config_list;
   Ecore_Exe_Event_Data *read;
   struct _config_exe_data *ced;
   char *value;
   char path[MAX_PATH];

   ev = event;
   if (!ev->exe)
      return 1;
   if (!(ecore_exe_tag_get(ev->exe) &&
         (!strcmp(ecore_exe_tag_get(ev->exe), "genmenu/fdo"))))
      return 1;
   paths = data;
   if (!paths)
      return 1;
   ced = ecore_exe_data_get(ev->exe);
   if (!ced)
      return 1;

   read = ecore_exe_event_data_get(ev->exe, ECORE_EXE_PIPE_READ);
   value = read->lines[0].line;
   config_list = fdo_paths_paths_to_list(value);
   if (config_list)
     {
        int i, j;

        for (i = 0; i < config_list->size; i++)
          {
             if (ced->types)
               {
                  for (j = 0; j < ced->types->size; j++)
                    {
                       _fdo_paths_massage_path(path, ced->home,
                                               config_list->list[i],
                                               ced->types->list[j]);
                       _fdo_paths_check_and_add(paths, path);
                    }
               }
             else
               {
                  _fdo_paths_massage_path(path, ced->home, config_list->list[i],
                                          NULL);
                  _fdo_paths_check_and_add(paths, path);
               }
          }
        E_FN_DEL(_fdo_paths_list_del, config_list);
     }

   ced->done = 1;
   return 1;
}
