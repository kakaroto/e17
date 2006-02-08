#ifndef _DUMB_LIST_H
# define _DUMB_LIST_H

#define E_FN_DEL(_fn, _h) if (_h) { _fn(_h); _h = NULL; }
#define E_REALLOC(p, s, n) p = (s *)realloc(p, sizeof(s) * n)
#define E_NEW(s, n) (s *)calloc(n, sizeof(s))
#define E_NEW_BIG(s, n) (s *)malloc(n * sizeof(s))
#define E_FREE(p) { if (p) {free(p); p = NULL;} }


enum _Dumb_List_Element_Type
{

   DUMB_LIST_ELEMENT_TYPE_STRING = 1,
   DUMB_LIST_ELEMENT_TYPE_LIST = 2,
};
typedef enum _Dumb_List_Element_Type Dumb_List_Element_Type;

struct _Dumb_List_Element
{
   void *element;                 /* A pointer to the element. */
   Dumb_List_Element_Type type;   /* The type of the element. */
};
typedef struct _Dumb_List_Element Dumb_List_Element;

struct _Dumb_List
{
   Dumb_List_Element *elements;  /* An array of elements. */
   int size;                     /* The size of the array. */
   char **buffers;               /* An array of pointers to the bits of data. */
   int buffers_size;             /* The size of the array. */
};
typedef struct _Dumb_List Dumb_List;

# ifdef __cplusplus
extern "C"
{
# endif

   Dumb_List *dumb_list_from_paths(char *paths);
   Dumb_List *dumb_list_new(char *buffer);
   Dumb_List *dumb_list_add(Dumb_List *list, char *element);
   Dumb_List *dumb_list_extend(Dumb_List *list, char *element);
   Dumb_List *dumb_list_add_child(Dumb_List *list, Dumb_List *element);
   int dumb_list_exist(Dumb_List *list, char *element);
   void dumb_list_dump(Dumb_List *list, int level);
   void dumb_list_del(Dumb_List *list);

# ifdef __cplusplus
}
# endif

#endif
