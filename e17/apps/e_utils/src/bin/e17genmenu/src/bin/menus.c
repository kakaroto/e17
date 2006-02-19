#include "global.h"
#include "config.h"
#include "fdo_menus.h"
#include "fdo_paths.h"
#include "parse.h"
#include "menus.h"
#include "xmlame.h"

extern double convert;
extern int menu_count, item_count;


static int _menu_make_apps(const void *data, Dumb_List *list, int element, int level);
static void _menu_dump_each_hash_node(void *value, void *user_data);


void
make_menus()
{
   char *d;

   if (get_fdo())
      {
         char *menu = "applications.menu";
         char *menu_file;

         /* First, find the main menu file. */
         menu_file = fdo_paths_search_for_file(FDO_PATHS_TYPE_MENU, menu, 1, NULL, NULL);
         if (menu_file)
            {
	       char *path;
	       Dumb_List *menu_xml = NULL;

               path = ecore_file_get_dir(menu_file);
               menu_xml = xmlame_get(menu_file);;
	       if ((menu_xml) && (path))
	          {
	             Dumb_List *menus = NULL;

	             /* convert the xml into menus */
	             menus = fdo_menus_get(menu_file, menu_xml);
		     convert = ecore_time_get();
	             if (menus)
	                {
	                   /* create the .eap and order files from the menu */
                           dumb_list_foreach(menu_xml, 0, _menu_make_apps, path);
		        }
	          }
               E_FREE(path);
            }
      }
   else
      {
         d = get_desktop_dir();
         if (d)
            check_for_dirs(strdup(d));

         if (!d)
           {
              /* Check desktop files in these directories */
              check_for_dirs(GNOME_DIRS);
              check_for_dirs(KDE_DIRS);
              check_for_dirs(DEBIAN_DIRS);
           }
         if (d)
            free(d);
      }
}

void
check_for_dirs(char *path)
{
   char *dir;
   char dirs[MAX_PATH];

   snprintf(dirs, sizeof(dirs), path);
   dir = path;   //strdup(path);
   if (dirs)
     {
        if (strstr(dirs, ":"))
           dir = strtok(dirs, ":");
     }
   if (!dir)
      return;
   while (dir)
     {
//#ifdef DEBUG
        fprintf(stderr, "\nChecking For %s...\n", dir);
//#endif
        if (ecore_file_exists(dir))
           check_for_files(strdup(dir));

        if (!strstr(dirs, ":"))
           break;
        dir = strtok(NULL, ":");
     }
//   if (dir)
//      free(dir);
}

void
check_for_files(char *dir)
{
   char *file;
   char path[MAX_PATH];
   Ecore_List *files = NULL;

#ifdef DEBUG
   fprintf(stderr, "Checking For Files In %s\n", dir);
#endif

   files = ecore_file_ls(dir);
   ecore_list_goto_first(files);
   while ((file = ecore_list_next(files)) != NULL)
     {
        snprintf(path, sizeof(path), "%s/%s", dir, file);
        if (!ecore_file_is_dir(path))
          {
             if (!strcmp(file + strlen(file) - 8, ".desktop"))
               {
                  if (!strcmp(file, "panel_appearance.desktop"))
                     return;
                  if (!strcmp(file, "ktip.desktop"))
                     return;
                  if (!strcmp(file, "kalarm.desktop"))
                     return;
                  /* Parse Desktop File */
                  parse_desktop_file(strdup(path), NULL);
               }
             else if (strstr(DEBIAN_DIRS, dir) != NULL)
               {
                  /* Parse Debian File */
                  if (strstr(file, "README") == NULL)
                     parse_debian_file(strdup(path));
               }
          }
     }
   if (files)
      ecore_list_destroy(files);
   if (file)
      free(file);
}

static int
_menu_make_apps(const void *data, Dumb_List *list, int element, int level)
{
   if (list->elements[element].type == DUMB_LIST_ELEMENT_TYPE_STRING)
      {
         if (strncmp((char *) list->elements[element].element, "<MENU ", 6) == 0)
	    {
               char *name, *path;
               Ecore_Hash *pool, *apps;

               name = (char *) list->elements[element].element;
               path = (char *) list->elements[element + 1].element;
               pool  = (Ecore_Hash *) list->elements[element + 2].element;
               apps  = (Ecore_Hash *) list->elements[element + 4].element;
               printf("MAKING MENU - %s \t\t%s\n", path, name);
	       menu_count++;
               ecore_hash_for_each_node(apps, _menu_dump_each_hash_node, &path[11]);
	    }
      }
   return 0;
}

static void
_menu_dump_each_hash_node(void *value, void *user_data)
{
   Ecore_Hash_Node *node;
   char *file, *path;

   path = (char *) user_data;
   node = (Ecore_Hash_Node *) value;
   file = (char *) node->value;
   printf("MAKING EAP %s -> %s\n", path, file);
   item_count++;
   parse_desktop_file(strdup(file), path);
}
