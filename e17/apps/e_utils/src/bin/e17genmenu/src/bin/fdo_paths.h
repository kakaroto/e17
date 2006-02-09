#ifndef _FDO_PATHS_H
# define _FDO_PATHS_H

#include "dumb_list.h"

enum _Fdo_Paths_Type
{

   FDO_PATHS_TYPE_MENU = 1,
   FDO_PATHS_TYPE_DIRECTORY = 2,
   FDO_PATHS_TYPE_DESKTOP = 4,
   FDO_PATHS_TYPE_ICON = 8,
};
typedef enum _Fdo_Paths_Type Fdo_Paths_Type;

Dumb_List *fdo_paths_menus;
Dumb_List *fdo_paths_directories;
Dumb_List *fdo_paths_desktops;
Dumb_List *fdo_paths_icons;

# ifdef __cplusplus
extern "C"
{
# endif

   void fdo_paths_init();
   char *fdo_paths_search_for_file(Fdo_Paths_Type type, char *file, int sub, int (*func) (const void *data, char *path), const void *data);
   char *fdo_paths_recursive_search(char *path, char *d, int (*func) (const void *data, char *path), const void *data);
   void fdo_paths_shutdown();

# ifdef __cplusplus
}
# endif

#endif
