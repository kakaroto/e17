#include "global.h"
#include "config.h"
#include "parse.h"
#include "menus.h"

void
make_menus()
{
   char *d;

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

void
check_for_dirs(char *path)
{
   char *dir;
   char dirs[MAX_PATH];

   snprintf(dirs, sizeof(dirs), path);
   dir = strdup(path);
   if (dirs)
     {
        if (strstr(dirs, ":"))
           dir = strtok(dirs, ":");
     }
   if (!dir)
      return;
   while (dir)
     {
#ifdef DEBUG
        fprintf(stderr, "\nChecking For %s...\n", dir);
#endif
        if (ecore_file_exists(dir))
           check_for_files(strdup(dir));

        if (!strstr(dirs, ":"))
           break;
        dir = strtok(NULL, ":");
     }
   if (dir)
      free(dir);
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
                  parse_desktop_file(strdup(path));
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
