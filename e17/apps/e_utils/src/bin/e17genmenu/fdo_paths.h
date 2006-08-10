#ifndef _FDO_PATHS_H
# define _FDO_PATHS_H

#include <Ecore_Data.h>

#define E_FN_DEL(_fn, _h) if (_h) { _fn(_h); _h = NULL; }
#define E_REALLOC(p, s, n) p = (s *)realloc(p, sizeof(s) * n)
#define E_NEW(s, n) (s *)calloc(n, sizeof(s))
#define E_NEW_BIG(s, n) (s *)malloc(n * sizeof(s))
#define E_FREE(p) { if (p) {free(p); p = NULL;} }

enum _Fdo_Paths_Type
{

   FDO_PATHS_TYPE_MENU = 1,
   FDO_PATHS_TYPE_DIRECTORY = 2,
   FDO_PATHS_TYPE_DESKTOP = 4,
   FDO_PATHS_TYPE_ICON = 8,
};
typedef enum _Fdo_Paths_Type Fdo_Paths_Type;

Ecore_List *fdo_paths_config;
Ecore_List *fdo_paths_menus;
Ecore_List *fdo_paths_directories;
Ecore_List *fdo_paths_desktops;
Ecore_List *fdo_paths_icons;
Ecore_List *fdo_paths_kde_legacy;

# ifdef __cplusplus
extern "C"
{
# endif

   void fdo_paths_init(void);
   char *fdo_paths_search_for_file(Fdo_Paths_Type type, char *file, int sub, int (*func) (const void *data, char *path),
                                   const void *data);
   char *fdo_paths_recursive_search(char *path, char *d, int (*dir_func) (const void *data, char *path),
                                    int (*func) (const void *data, char *path), const void *data);
   void fdo_paths_shutdown(void);

   Ecore_Hash *ecore_hash_from_paths(char *paths);
   Ecore_List *ecore_list_from_paths(char *paths);


# ifdef __cplusplus
}
# endif

#endif
