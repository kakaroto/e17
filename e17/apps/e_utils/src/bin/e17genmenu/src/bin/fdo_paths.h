#ifndef _FDO_PATHS_H
# define _FDO_PATHS_H

#include "dumb_tree.h"

enum _Fdo_Paths_Type
{

   FDO_PATHS_TYPE_MENU = 1,
   FDO_PATHS_TYPE_DIRECTORY = 2,
   FDO_PATHS_TYPE_DESKTOP = 4,
   FDO_PATHS_TYPE_ICON = 8,
};
typedef enum _Fdo_Paths_Type Fdo_Paths_Type;

Dumb_Tree *fdo_paths_config;
Dumb_Tree *fdo_paths_menus;
Dumb_Tree *fdo_paths_directories;
Dumb_Tree *fdo_paths_desktops;
Dumb_Tree *fdo_paths_icons;
Dumb_Tree *fdo_paths_kde_legacy;

# ifdef __cplusplus
extern "C"
{
# endif

   void fdo_paths_init(void);
   char *fdo_paths_search_for_file(Fdo_Paths_Type type, char *file, int sub, int (*func) (const void *data, char *path), const void *data);
   char *fdo_paths_recursive_search(char *path, char *d, int (*dir_func) (const void *data, char *path), int (*func) (const void *data, char *path), const void *data);
   void fdo_paths_shutdown(void);

# ifdef __cplusplus
}
# endif

#endif
