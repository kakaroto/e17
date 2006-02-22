#ifndef _DUMB_TREE_H
# define _DUMB_TREE_H

#define E_FN_DEL(_fn, _h) if (_h) { _fn(_h); _h = NULL; }
#define E_REALLOC(p, s, n) p = (s *)realloc(p, sizeof(s) * n)
#define E_NEW(s, n) (s *)calloc(n, sizeof(s))
#define E_NEW_BIG(s, n) (s *)malloc(n * sizeof(s))
#define E_FREE(p) { if (p) {free(p); p = NULL;} }

#include <Ecore_Data.h>


typedef enum _Dumb_Tree_Element_Type Dumb_Tree_Element_Type;
enum _Dumb_Tree_Element_Type
{
   DUMB_TREE_ELEMENT_TYPE_NULL = 0,
   DUMB_TREE_ELEMENT_TYPE_STRING = 1,
   DUMB_TREE_ELEMENT_TYPE_TREE = 2,
   DUMB_TREE_ELEMENT_TYPE_HASH = 3,
};

typedef struct _Dumb_Tree_Element Dumb_Tree_Element;
struct _Dumb_Tree_Element
{
   void *element;                 /* A pointer to the element. */
   Dumb_Tree_Element_Type type;   /* The type of the element. */
};

typedef struct _Dumb_Tree Dumb_Tree;
struct _Dumb_Tree
{
   Dumb_Tree_Element *elements;  /* An array of elements. */
   int size;                     /* The size of the array. */
   char **buffers;               /* An array of pointers to the bits of data. */
   int buffers_size;             /* The size of the array. */
   Dumb_Tree *parent;            /* Parent if this is a child. */
};

# ifdef __cplusplus
extern "C"
{
# endif

   Dumb_Tree *dumb_tree_from_paths(char *paths);
   Dumb_Tree *dumb_tree_new(char *buffer);
   Dumb_Tree *dumb_tree_add(Dumb_Tree *tree, char *element);
   void       dumb_tree_track(Dumb_Tree *tree, void *element);
   Dumb_Tree *dumb_tree_extend(Dumb_Tree *tree, char *element);
   Dumb_Tree *dumb_tree_insert(Dumb_Tree *tree, int before, void *element, Dumb_Tree_Element_Type type);
   Dumb_Tree *dumb_tree_insert_tree(Dumb_Tree *tree, int before, Dumb_Tree *element);
   Dumb_Tree *dumb_tree_add_child(Dumb_Tree *tree, Dumb_Tree *element);
   Dumb_Tree *dumb_tree_add_hash(Dumb_Tree *tree, Ecore_Hash *element);
   int        dumb_tree_exist(Dumb_Tree *tree, char *element);
   int        dumb_tree_foreach(Dumb_Tree *tree, int level, int (*func) (const void *data, Dumb_Tree *tree, int element, int level), const void *data);
   void       dumb_tree_dump(Dumb_Tree *tree, int level);
   void       dumb_tree_del(Dumb_Tree *tree);

# ifdef __cplusplus
}
# endif

#endif
