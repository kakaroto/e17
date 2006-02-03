#ifndef _FDO_PATHS_H
# define _FDO_PATHS_H

enum _Fdo_Paths_Type
{

   FDO_PATHS_TYPE_MENU = 1,
   FDO_PATHS_TYPE_DIRECTORY = 2,
   FDO_PATHS_TYPE_DESKTOP = 4,
   FDO_PATHS_TYPE_ICON = 8,
};
typedef enum _Fdo_Paths_Type Fdo_Paths_Type;

struct _Fdo_Path_List
{
   char **list;                 /* An array of pointers to the paths. */
   int size;                    /* The size of the array. */
   char *buffer;                /* The actual path data. */
   int length;                  /* Length of the buffer. */
};
typedef struct _Fdo_Path_List Fdo_Path_List;

Fdo_Path_List *fdo_paths_menus;
Fdo_Path_List *fdo_paths_directories;
Fdo_Path_List *fdo_paths_desktops;
Fdo_Path_List *fdo_paths_icons;

# ifdef __cplusplus
extern "C"
{
# endif

   void fdo_paths_init();
   char *fdo_paths_search_for_file(Fdo_Paths_Type type, char *file, int sub, int (*func) (const void *data, char *path), const void *data);
   Fdo_Path_List *fdo_paths_paths_to_list(char *paths);
   void fdo_paths_shutdown();

# ifdef __cplusplus
}
# endif

#endif
