/*
 * This conforms with the freedesktop.org XDG Base Directory Specification version 0.6
 */

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

struct _config_exe_data
{
   char *home;
   Dumb_Tree *types;
   int done;
};

static Dumb_Tree *_fdo_paths_get(char *before, char *env_home, char *env,
                                char *env_home_default, char *env_default,
                                char *type, char *gnome_extra, char *kde);
static void _fdo_paths_massage_path(char *path, char *home, char *first,
                                    char *second);
static void _fdo_paths_check_and_add(Dumb_Tree * paths, char *path);
static void _fdo_paths_exec_config(char *home, Dumb_Tree * extras,
                                   char *cmd);

static int _fdo_paths_cb_exe_exit(void *data, int type, void *event);

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
   if (!fdo_paths_kde_legacy)
      fdo_paths_kde_legacy = _fdo_paths_get(NULL, NULL, NULL, NULL, NULL, NULL, NULL, "apps");
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
   E_FN_DEL(dumb_tree_del, fdo_paths_menus);
   E_FN_DEL(dumb_tree_del, fdo_paths_directories);
   E_FN_DEL(dumb_tree_del, fdo_paths_desktops);
   E_FN_DEL(dumb_tree_del, fdo_paths_icons);
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
   Dumb_Tree *paths = NULL;

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
        sprintf(temp, "%s%s", (char *) paths->elements[i].element, file);
        if (stat(temp, &path_stat) == 0)
          {
             path = strdup(temp);
             if (func)
                if (func(data, path))
                   break;
          }
        else if (sub)
           path = fdo_paths_recursive_search(paths->elements[i].element, file, func, data);
        if (path && (!func))
           break;
     }

   return path;
}


/*  We need -
config file full of paths
menus=pathlist
desktops=pathlist
directories=pathlist
icons=pathlist
*/


static Dumb_Tree *
_fdo_paths_get(char *before, char *env_home, char *env, char *env_home_default,
               char *env_default, char *type, char *gnome_extra, char *kde)
{
   char *home;
   Dumb_Tree *paths = NULL;
   Dumb_Tree *types;
   Dumb_Tree *gnome_extras;
   Dumb_Tree *kdes;
   Ecore_Event_Handler *exit_handler;

   /* Don't sort them, as they are in preferred order from each source. */
   /* Merge the results, there are probably some duplicates. */

   types = dumb_tree_from_paths(type);
   gnome_extras = dumb_tree_from_paths(gnome_extra);
   kdes = dumb_tree_from_paths(kde);

   home = get_home();
   if (home)
     {
        int last;

        /* Strip traling slash of home. */
        last = strlen(home) - 1;
        if ((last >= 0) && (home[last] == '/'))
           home[last] = '\0';
     }

   paths = dumb_tree_new(NULL);
   if (paths)
     {
        int i, j;
        char path[MAX_PATH];
        Dumb_Tree *env_list;

        if (before)
          {
             Dumb_Tree *befores;

             befores = dumb_tree_from_paths(before);
             if (befores)
               {
                  for (i = 0; i < befores->size; i++)
                    {
                       _fdo_paths_massage_path(path, home,
                                               befores->elements[i].element, NULL);
                       _fdo_paths_check_and_add(paths, path);
                    }
                  E_FN_DEL(dumb_tree_del, befores);
               }
          }

        if (env_home)
          {
             char *value;

             value = getenv(env_home);
             if ((value == NULL) || (value[0] == '\0'))
                value = env_home_default;
             env_list = dumb_tree_from_paths(value);
             if (env_list)
               {
                  for (i = 0; i < env_list->size; i++)
                    {
                       for (j = 0; j < types->size; j++)
                         {
                            _fdo_paths_massage_path(path, home,
                                                    env_list->elements[i].element,
                                                    types->elements[j].element);
                            _fdo_paths_check_and_add(paths, path);
                         }
                    }
                  E_FN_DEL(dumb_tree_del, env_list);
               }
          }

        if (env)
          {
             char *value;

             value = getenv(env);
             if ((value == NULL) || (value[0] == '\0'))
                value = env_default;
             env_list = dumb_tree_from_paths(value);
             if (env_list)
               {
                  for (i = 0; i < env_list->size; i++)
                    {
                       for (j = 0; j < types->size; j++)
                         {
                            _fdo_paths_massage_path(path, home,
                                                    env_list->elements[i].element,
                                                    types->elements[j].element);
                            _fdo_paths_check_and_add(paths, path);
                         }
                    }
                  E_FN_DEL(dumb_tree_del, env_list);
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

             sprintf(cmd, "kde-config --path %s", (char *) kdes->elements[i].element);
             _fdo_paths_exec_config(home, NULL, cmd);
          }

        ecore_event_handler_del(exit_handler);
     }

   E_FREE(home);
   E_FN_DEL(dumb_tree_del, kdes);
   E_FN_DEL(dumb_tree_del, gnome_extras);
   E_FN_DEL(dumb_tree_del, types);

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
_fdo_paths_check_and_add(Dumb_Tree * paths, char *path)
{
#ifdef DEBUG
   printf("CHECKING %s", path);
#endif
   if (!dumb_tree_exist(paths, path))
     {
        struct stat path_stat;

        /* Check if the path exists. */
        if ((stat(path, &path_stat) == 0) && (S_ISDIR(path_stat.st_mode)))
          {
#ifdef DEBUG
             printf(" OK");
#endif
             dumb_tree_extend(paths, path);
          }
     }
#ifdef DEBUG
   printf("\n");
#endif
}

static void
_fdo_paths_exec_config(char *home, Dumb_Tree *extras, char *cmd)
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
	     /* FIXME: raster is paranoid.  If too much time passes, give up.
	      * Or find a way to let the usual event loop shit do this without spinning our wheels.
	      */
             ecore_main_loop_iterate();
             usleep(10);
          }
     }
}

char *
fdo_paths_recursive_search(char *path, char *file,
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
                               fdo_paths_recursive_search(info_text, file,
                                                           func, data);
                         }
                    }
                  else
                    {
		       if (file)
		          {
                             if (strcmp(basename(info_text), file) == 0)
                               {
                                  fpath = strdup(info_text);
                                  if (func)
                                     if (func(data, path))
                                        break;
                               }
			  }
		       else
		          {
                              if (func)
                                 if (func(data, info_text))
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
   Dumb_Tree *paths;
   Dumb_Tree *config_list;
   Ecore_Exe_Event_Data *read;
   struct _config_exe_data *ced;
   char *value;
   char path[MAX_PATH];

   ev = event;
   if (!ev->exe)
      return 1;
   value = ecore_exe_tag_get(ev->exe);
   if ((!value) || (strcmp(value, "genmenu/fdo")) != 0)
      return 1;
   paths = data;
   if (!paths)
      return 1;
   ced = ecore_exe_data_get(ev->exe);
   if (!ced)
      return 1;

   read = ecore_exe_event_data_get(ev->exe, ECORE_EXE_PIPE_READ);
   if ((read) && (read->lines[0].line))
      {
         value = read->lines[0].line;
         if (value)
            {
               config_list = dumb_tree_from_paths(value);
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
                                               config_list->elements[i].element,
                                               ced->types->elements[j].element);
                                       _fdo_paths_check_and_add(paths, path);
                                    }
                              }
                           else
                              {
                                 _fdo_paths_massage_path(path, ced->home, config_list->elements[i].element,
                                          NULL);
                                 _fdo_paths_check_and_add(paths, path);
                              }
                        }
                     E_FN_DEL(dumb_tree_del, config_list);
                  }
            }
      }
   ced->done = 1;
   return 1;
}
