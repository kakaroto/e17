#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include "dumb_tree.h"

static void dumb_tree_dump_each_hash_node(void *value, void *user_data);

/** Split a list of paths into a Dumb_Tree.
 *
 * The list of paths can use any one of ;:, to seperate the paths.
 * You can also escape the :;, with \.
 *
 * FIXME: The concept here is still buggy, but it should do for now.
 *
 * @param   paths A list of paths.
 */
Dumb_Tree *
dumb_tree_from_paths(char *paths)
{
   Dumb_Tree *tree = NULL;

   tree = dumb_tree_new(paths);
   if ((tree) && (tree->buffers))
     {
        char *start, *end;
        int finished = 0;

        end = tree->buffers[0];
        while (!finished)
          {
             start = end;
             do                 /* FIXME: There is probably a better way to do this. */
               {
                  while ((*end != ';') && (*end != ':') && (*end != ',') && (*end != '\0'))
                     end++;
               }
             while ((end != tree->buffers[0]) && (*(end - 1) == '\\') && (*end != '\0'));       /* Ignore any escaped ;:, */
             /* FIXME: We still need to unescape it now. */
             if (*end == '\0')
                finished = 1;
             *end = '\0';
             if (!dumb_tree_exist(tree, start))
                tree = dumb_tree_add(tree, start);
             end++;
          }
     }
   return tree;
}

/* Just a quick and dirty tree implemtation that will likely get replaced by 
 * something much saner at a later date.  I wrote most of this while falling
 * asleep.  It will probably scare me when I wake up.  B-)
 *
 * Devilhorns said to make it portable, so we can't rely on any external tree 
 * implementation.  So this tree is designed specifically for this task.  Then
 * we finally found a place for the genmenu code, and Ecore was back on the 
 * menu.  However, speed could be an issue later, so it might be worth it to 
 * stick with a custom tree implementation, so that we can optimize it for this
 * task.
 *
 * The trees will be tiny.
 * They only store strings.
 * There is no insertion or deletion, only append.
 * Append order must be maintained.
 * The trees will only ever be accessed sequentially, from begining to end.
 * The tree data will come in two ways, all in one big string, or a bunch of 
 * seperate strings, one per element.  Any particular tree might have both.
 *
 * No duplicates in the tree,
 *    This is the nasty part of this tree implementation.
 *    Insertions involve a linear search for dupes, most of the 
 *    time there won't be any dupes, so the tree is searched in
 *    it's entirety.  These trees will be really small, and only created at 
 *    the begining, so no big drama there.
 * The tree may allow duplicates.
 */

Dumb_Tree *
dumb_tree_new(char *buffer)
{
   Dumb_Tree *tree;

   tree = E_NEW(Dumb_Tree, 1);
   if ((tree) && (buffer))
     {
        tree->buffers = (char **)realloc(tree->buffers, (tree->buffers_size + 1) * sizeof(char *));
        tree->buffers[tree->buffers_size++] = strdup(buffer);
     }
   return tree;
}

Dumb_Tree *
dumb_tree_add(Dumb_Tree * tree, char *element)
{
   tree->elements = (Dumb_Tree_Element *) realloc(tree->elements, (tree->size + 1) * sizeof(Dumb_Tree_Element));
   tree->elements[tree->size].element = element;
   tree->elements[tree->size++].type = DUMB_TREE_ELEMENT_TYPE_STRING;
   return tree;
}

Dumb_Tree *
dumb_tree_extend(Dumb_Tree * tree, char *element)
{
   tree->buffers = (char **)realloc(tree->buffers, (tree->buffers_size + 1) * sizeof(char *));
   tree->buffers[tree->buffers_size++] = strdup(element);
   tree = dumb_tree_add(tree, tree->buffers[tree->buffers_size - 1]);

   return tree;
}

void
dumb_tree_track(Dumb_Tree * tree, void *element)
{
   tree->buffers = (char **)realloc(tree->buffers, (tree->buffers_size + 1) * sizeof(char *));
   tree->buffers[tree->buffers_size++] = element;
}

/* OK, so we need an insert after all, and it falls into the dumb category. */
Dumb_Tree *
dumb_tree_insert(Dumb_Tree * tree, int before, void *element, Dumb_Tree_Element_Type type)
{
   int i;

   tree->elements = (Dumb_Tree_Element *) realloc(tree->elements, (tree->size + 1) * sizeof(Dumb_Tree_Element));
   tree->size++;
   for (i = tree->size - 1; i > before; i--)
     {
        tree->elements[i].element = tree->elements[i - 1].element;
        tree->elements[i].type = tree->elements[i - 1].type;
     }
   tree->elements[before].element = element;
   tree->elements[before].type = type;
   return tree;
}

/* OK, so we need a tree merge after all, and it falls into the dumb category. */
Dumb_Tree *
dumb_tree_merge(Dumb_Tree * tree, int before, Dumb_Tree * element)
{
   int i, size;

   size = element->size;
   if (size)
     {
        tree->elements = (Dumb_Tree_Element *) realloc(tree->elements, (tree->size + size) * sizeof(Dumb_Tree_Element));
        tree->size += size;
        for (i = tree->size - 1; i > before; i--)
          {
             tree->elements[i].element = tree->elements[i - size].element;
             tree->elements[i].type = tree->elements[i - size].type;
          }
        for (i = 0; i < size; i++)
          {
             tree->elements[before + i].element = element->elements[i].element;
             tree->elements[before + i].type = element->elements[i].type;
          }
     }

   /* Careful, this might screw up the freeing order if that is important. */
   size = element->buffers_size;
   if (size)
     {
/*
         tree->buffers = (char **) realloc(tree->buffers, (tree->buffers_size + size) * sizeof(char *));
         tree->buffers_size += size;
         for (i = 0; i < size; i++)
            {
               tree->buffers[tree->buffers_size + i] = element->buffers[i];
	       element->buffers[i] = NULL;
            }
*/
     }
   return tree;
}

Dumb_Tree *
dumb_tree_add_child(Dumb_Tree * tree, Dumb_Tree * element)
{
   tree->elements = (Dumb_Tree_Element *) realloc(tree->elements, (tree->size + 1) * sizeof(Dumb_Tree_Element));
   tree->elements[tree->size].element = element;
   tree->elements[tree->size++].type = DUMB_TREE_ELEMENT_TYPE_TREE;
   element->parent = tree;
   return tree;
}

Dumb_Tree *
dumb_tree_add_hash(Dumb_Tree * tree, Ecore_Hash * element)
{
   tree->elements = (Dumb_Tree_Element *) realloc(tree->elements, (tree->size + 1) * sizeof(Dumb_Tree_Element));
   tree->elements[tree->size].element = element;
   tree->elements[tree->size++].type = DUMB_TREE_ELEMENT_TYPE_HASH;
   return tree;
}

void
dumb_tree_remove(Dumb_Tree * tree, int element)
{
   if (tree->size > element)
     {
        tree->elements[element].type = DUMB_TREE_ELEMENT_TYPE_NULL;
        tree->elements[element].element = NULL;
     }
}

int
dumb_tree_exist(Dumb_Tree * tree, char *element)
{
   int exist = 0;
   int i;

   /* This is the dumb part of the tree, a linear search. */
   for (i = 0; i < tree->size; i++)
     {
        if ((tree->elements[i].type == DUMB_TREE_ELEMENT_TYPE_STRING) && (strcmp((char *)tree->elements[i].element, element) == 0))
          {
             exist = 1;
             break;
          }
     }
   return exist;
}

int
dumb_tree_foreach(Dumb_Tree * tree, int level, int (*func) (const void *data, Dumb_Tree * tree, int element, int level), const void *data)
{
   int result = 0;
   int i;

   for (i = 0; i < tree->size; i++)
     {
        if (tree->elements[i].type == DUMB_TREE_ELEMENT_TYPE_TREE)
          {
             if (dumb_tree_foreach((Dumb_Tree *) tree->elements[i].element, level + 1, func, data))
                result = 1;
          }
        else if (tree->elements[i].type == DUMB_TREE_ELEMENT_TYPE_NULL)
          {
             /* This falls into the dumb category. */
             int j = i;
             int k = i;
             int moved = 0;

             /* Find the next non NULL element. */
             while ((j < tree->size) && (tree->elements[j].type == DUMB_TREE_ELEMENT_TYPE_NULL))
                j++;
             /* Move the next batch of non NULL up. */
             while ((j < tree->size) && (tree->elements[j].type != DUMB_TREE_ELEMENT_TYPE_NULL))
               {
                  moved = 1;
                  tree->elements[k].type = tree->elements[j].type;
                  tree->elements[k].element = tree->elements[j].element;
                  tree->elements[j].type = DUMB_TREE_ELEMENT_TYPE_NULL;
                  tree->elements[j].element = NULL;
                  j++;
                  k++;
               }
             if (moved)
                i--;
             else
                tree->size = i;
          }
        else
          {
             if (func(data, tree, i, level))
                result = 1;
          }
     }
   return result;
}

void
dumb_tree_dump(Dumb_Tree * tree, int level)
{
   int i;

   for (i = 0; i < tree->size; i++)
     {
        int j;

        for (j = 0; j < level; j++)
           printf(".");
        switch (tree->elements[i].type)
          {
          case DUMB_TREE_ELEMENT_TYPE_NULL:
             {
                printf("NULL\n");
             }
             break;

          case DUMB_TREE_ELEMENT_TYPE_STRING:
             {
                printf("%s\n", (char *)tree->elements[i].element);
             }
             break;

          case DUMB_TREE_ELEMENT_TYPE_TREE:
             {
                printf("TREE ELEMENT TYPE\n");
                dumb_tree_dump((Dumb_Tree *) tree->elements[i].element, level + 1);
             }
             break;

          case DUMB_TREE_ELEMENT_TYPE_HASH:
             {
                int lev;

                lev = level + 1;
                printf("HASH ELEMENT TYPE\n");
                ecore_hash_for_each_node((Ecore_Hash *) tree->elements[i].element, dumb_tree_dump_each_hash_node, &lev);
             }
             break;

          default:
             {
                printf("UNKNOWN ELEMENT TYPE!\n");
             }
             break;
          }
     }
}

static void
dumb_tree_dump_each_hash_node(void *value, void *user_data)
{
   Ecore_Hash_Node *node;
   int level;
   int j;

   node = (Ecore_Hash_Node *) value;
   level = *((int *)user_data);
   for (j = 0; j < level; j++)
      printf(".");
   printf("%s = %s\n", (char *)node->key, (char *)node->value);
}

void
dumb_tree_del(Dumb_Tree * tree)
{
   int i;

   for (i = tree->size - 1; i >= 0; i--)
     {
        if (tree->elements[i].type == DUMB_TREE_ELEMENT_TYPE_TREE)
           dumb_tree_del((Dumb_Tree *) tree->elements[i].element);
        else if (tree->elements[i].type == DUMB_TREE_ELEMENT_TYPE_HASH)
           ecore_hash_destroy((Ecore_Hash *) tree->elements[i].element);
     }

   E_FREE(tree->elements);

   for (i = tree->buffers_size - 1; i >= 0; i--)
      E_FREE(tree->buffers[i]);
   E_FREE(tree);
}
