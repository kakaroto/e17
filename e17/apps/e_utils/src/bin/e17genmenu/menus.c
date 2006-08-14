#include "global.h"
#include "config.h"
#include "parse.h"
#include "menus.h"

//#define DEBUG 1

extern int menu_count, item_count;
extern double generate_time;

static int _menu_make_apps(const void *data, Ecore_Desktop_Tree * tree, int element, int level);
static void _menu_dump_each_hash_node(void *value, void *user_data);

void
make_menus()
{
   char *d;
   char *menu = "applications.menu";
   char *menu_file;
   double begin;

   d = get_desktop_dir();
   if ((d) && (!get_fdo()))
      check_for_dirs(strdup(d));

   if (!d)
     {
         if (!get_fdo())
	   {
	      char *this_path;

              printf("Generating menus.\n");
              begin = ecore_time_get();
              /* Check desktop files in these directories */
              check_for_dirs(GNOME_DIRS);    /* FIXME: probably obsolete. */
              check_for_dirs(KDE_DIRS);      /* FIXME: probably obsolete. */
              check_for_dirs(DEBIAN_DIRS);   /* FIXME: may or may not be obsolete. */

              ecore_list_goto_first(ecore_desktop_paths_desktops);
              while ((this_path = ecore_list_next(ecore_desktop_paths_desktops)) != NULL)
                 check_for_dirs(this_path);
              ecore_list_goto_first(ecore_desktop_paths_kde_legacy);
              while ((this_path = ecore_list_next(ecore_desktop_paths_kde_legacy)) != NULL)
                 check_for_dirs(this_path);
              generate_time = ecore_time_get() - begin;
	   }

        printf("Converting freedesktop.org (fdo) menus.\n");
        /* Find the main menu file. */
        menu_file = ecore_desktop_paths_file_find(ecore_desktop_paths_menus, menu, 1, NULL, NULL);
        if (menu_file)
          {
             char *path;

             path = ecore_file_get_dir(menu_file);
             if (path)
               {
                  Ecore_Desktop_Tree *menus = NULL;

                  /* convert the xml into menus */
                  menus = ecore_desktop_menu_get(menu_file, NULL, 0);
                  if (menus)
                    {
                       /* create the .eap and order files from the menu */
                       ecore_desktop_tree_foreach(menus, 0, _menu_make_apps, path);
                    }
               }
             E_FREE(path);
          }

     }

   if (d)
      free(d);
}

void
check_for_dirs(char *path)
{
   char *dir;
   char dirs[MAX_PATH];

   snprintf(dirs, sizeof(dirs), path);
   dir = path;                  //strdup(path);
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
_menu_make_apps(const void *data, Ecore_Desktop_Tree * tree, int element, int level)
{
   if (tree->elements[element].type == ECORE_DESKTOP_TREE_ELEMENT_TYPE_STRING)
     {
        if (strncmp((char *)tree->elements[element].element, "<MENU ", 6) == 0)
          {
             char *name, *path;
             Ecore_Hash *pool, *apps;

             name = (char *)tree->elements[element].element;
             path = (char *)tree->elements[element + 1].element;
             pool = (Ecore_Hash *) tree->elements[element + 2].element;
             apps = (Ecore_Hash *) tree->elements[element + 4].element;
#ifdef DEBUG
             printf("MAKING MENU - %s \t\t%s\n", path, name);
#endif
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

   path = (char *)user_data;
   node = (Ecore_Hash_Node *) value;
   file = (char *)node->value;
#ifdef DEBUG
   printf("MAKING EAP %s -> %s\n", path, file);
#endif
   item_count++;
   parse_desktop_file(strdup(file), path);
}
