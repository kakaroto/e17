#include "global.h"
#include "config.h"
#include "parse.h"
#include "menus.h"
#include "e_fdo_menu_to_order.h"

//#define DEBUG 1

extern int menu_count, item_count;
extern double generate_time;


void
make_menus()
{
   char *d;
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
	e_fdo_menu_to_order();
     }

   if (d)
      free(d);
}

void
check_for_dirs(char *path)
{
   char *dir;
   char dirs[PATH_MAX];

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
   char path[PATH_MAX];
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
