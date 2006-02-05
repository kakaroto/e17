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

enum _Fdo_Element_Type
{

   FDO_ELEMENT_TYPE_STRING = 1,
   FDO_ELEMENT_TYPE_LIST = 2,
};
typedef enum _Fdo_Element_Type Fdo_Element_Type;

struct _Fdo_Element
{
   void *element;                 /* A pointer to the element. */
   Fdo_Element_Type type;   /* The type of the element. */
};
typedef struct _Fdo_Element Fdo_Element;

struct _Fdo_List
{
   Fdo_Element *elements;  /* An array of elements. */
   int size;               /* The size of the array. */
   char **buffers;         /* An array of pointers to the bits of data. */
   int buffers_size;       /* The size of the array. */
};
typedef struct _Fdo_List Fdo_List;

Fdo_List *fdo_paths_menus;
Fdo_List *fdo_paths_directories;
Fdo_List *fdo_paths_desktops;
Fdo_List *fdo_paths_icons;

# ifdef __cplusplus
extern "C"
{
# endif

   void fdo_paths_init();
   char *fdo_paths_search_for_file(Fdo_Paths_Type type, char *file, int sub, int (*func) (const void *data, char *path), const void *data);
   Fdo_List *fdo_paths_to_list(char *paths);
   void fdo_paths_shutdown();

# ifdef __cplusplus
}
# endif

#endif
