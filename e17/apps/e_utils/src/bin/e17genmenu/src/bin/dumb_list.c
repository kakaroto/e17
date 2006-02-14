#include <string.h>             //string funcs
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include "dumb_list.h"

static void dumb_list_dump_each_hash_node(void *value, void *user_data);


/** Split a list of paths into a Dumb_List.
 *
 * The list of paths can use any one of ;:, to seperate the paths.
 * You can also escape the :;, with \.
 *
 * FIXME: The concept here is still buggy, but it should do for now.
 *
 * @param   paths A list of paths.
 */
Dumb_List *
dumb_list_from_paths(char *paths)
{
   Dumb_List *list = NULL;

   list = dumb_list_new(paths);
   if ((list) && (list->buffers))
     {
        char *start, *end;
        int finished = 0;

        end = list->buffers[0];
        while (!finished)
          {
             start = end;
	     do   /* FIXME: There is probably a better way to do this. */
	        {
                   while ((*end != ';') && (*end != ':') && (*end != ',') && (*end != '\0'))
                      end++;
	        }
             while ((end != list->buffers[0]) && (*(end - 1) == '\\') && (*end != '\0'));  /* Ignore any escaped ;:, */
	     /* FIXME: We still need to unescape it now. */
             if (*end == '\0')
                finished = 1;
             *end = '\0';
             if (!dumb_list_exist(list, start))
                list = dumb_list_add(list, start);
             end++;
          }
     }
   return list;
}


/* Just a quick and dirty list implemtation that will likely get replaced by 
 * something much saner at a later date.  I wrote most of this while falling
 * asleep.  It will probably scare me when I wake up.  B-)
 *
 * Devilhorns said to make it portable, so we can't rely on any external list 
 * implementation.  So this list is designed specifically for this task.  Then
 * we finally found a place for the genmenu code, and Ecore was back on the 
 * menu.  However, speed could be an issue later, so it might be worth it to 
 * stick with a custom list implementation, so that we can optimize it for this
 * task.
 *
 * On top of all this, for parsing the XML metu files, a tree will be needed, 
 * and it's requirements are the same as the list requirements.  I simple tweak
 * let's this code do both.
 *
 * The lists will be tiny.
 * They only store strings.
 * There is no insertion or deletion, only append.
 * Append order must be maintained.
 * The lists will only ever be accessed sequentially, from begining to end.
 * The list data will come in two ways, all in one big string, or a bunch of 
 * seperate strings, one per element.  Any particular list might have both.
 *
 * No duplicates in the list,
 *    This is the nasty part of this list implementation.
 *    Insertions involve a linear search for dupes, most of the 
 *    time there won't be any dupes, so the list is searched in
 *    it's entirety.  These lists will be really small, and only created at 
 *    the begining, so no big drama there.
 * The tree may allow duplicates.
 */


Dumb_List *
dumb_list_new(char *buffer)
{
   Dumb_List *list;

   list = E_NEW(Dumb_List, 1);
   if ((list) && (buffer))
     {
        list->buffers = (char **) realloc(list->buffers, (list->buffers_size + 1) * sizeof(char *));
        list->buffers[list->buffers_size++] = strdup(buffer);
     }
   return list;
}

Dumb_List *
dumb_list_add(Dumb_List *list, char *element)
{
   list->elements = (Dumb_List_Element *) realloc(list->elements, (list->size + 1) * sizeof(Dumb_List_Element));
   list->elements[list->size].element = element;
   list->elements[list->size++].type = DUMB_LIST_ELEMENT_TYPE_STRING;
   return list;
}

Dumb_List *
dumb_list_extend(Dumb_List *list, char *element)
{
   list->buffers = (char **) realloc(list->buffers, (list->buffers_size + 1) * sizeof(char *));
   list->buffers[list->buffers_size++] = strdup(element);
   list = dumb_list_add(list, list->buffers[list->buffers_size - 1]);

   return list;
}

void
dumb_list_track(Dumb_List *list, void *element)
{
   list->buffers = (char **) realloc(list->buffers, (list->buffers_size + 1) * sizeof(char *));
   list->buffers[list->buffers_size++] = element;
}

Dumb_List *
dumb_list_add_child(Dumb_List *list, Dumb_List *element)
{
   list->elements = (Dumb_List_Element *) realloc(list->elements, (list->size + 1) * sizeof(Dumb_List_Element));
   list->elements[list->size].element = element;
   list->elements[list->size++].type = DUMB_LIST_ELEMENT_TYPE_LIST;
   element->parent = list;
   return list;
}

Dumb_List *
dumb_list_add_hash(Dumb_List *list, Ecore_Hash *element)
{
   list->elements = (Dumb_List_Element *) realloc(list->elements, (list->size + 1) * sizeof(Dumb_List_Element));
   list->elements[list->size].element = element;
   list->elements[list->size++].type = DUMB_LIST_ELEMENT_TYPE_HASH;
   return list;
}

int
dumb_list_exist(Dumb_List *list, char *element)
{
   int exist = 0;
   int i;

   /* This is the dumb part of the list, a linear search. */
   for (i = 0; i < list->size; i++)
     {
        if ((list->elements[i].type == DUMB_LIST_ELEMENT_TYPE_STRING) &&
            (strcmp((char *) list->elements[i].element, element) == 0))
          {
             exist = 1;
             break;
          }
     }
   return exist;
}

int
dumb_list_foreach(Dumb_List *list, int level, int (*func) (const void *data, Dumb_List *list, int element, int level), const void *data)
{
   int result = 0;
   int i;

   for (i = 0; i < list->size; i++)
      {
         if (list->elements[i].type == DUMB_LIST_ELEMENT_TYPE_LIST)
	    {
	       if (dumb_list_foreach((Dumb_List *) list->elements[i].element, level + 1, func, data))
                  result = 1;
	    }
	 else if (list->elements[i].type == DUMB_LIST_ELEMENT_TYPE_NULL)
	    {
	       int j = i;
	       int k = i;
	       int moved = 0;

               /* Find the next non NULL element. */
               while ((j < list->size) && (list->elements[j].type == DUMB_LIST_ELEMENT_TYPE_NULL))
	          j++;
	       /* Move the next batch of non NULL up. */
               while ((j < list->size) && (list->elements[j].type != DUMB_LIST_ELEMENT_TYPE_NULL))
	          {
		     moved = 1;
		     list->elements[k].type = list->elements[j].type;
		     list->elements[k].element = list->elements[j].element;
		     list->size--;
	             j++;
		     k++;
		  }
	       if (moved)
		  i--;
	       else
	          list->size = i;
	    }
	 else
	    {
	       if (func(data, list, i, level))
		  result = 1;
	    }
      }
   return result;
}

void
dumb_list_dump(Dumb_List *list, int level)
{
   int i;

   for (i = 0; i < list->size; i++)
      {
         int j;

         for (j = 0; j < level; j++)
	    printf(".");
         switch (list->elements[i].type)
	    {
	       case DUMB_LIST_ELEMENT_TYPE_NULL :
	          {
		     printf("NULL\n");
		  }
	          break;

	       case DUMB_LIST_ELEMENT_TYPE_STRING :
	          {
		     printf("%s\n", (char *) list->elements[i].element);
		  }
	          break;

	       case DUMB_LIST_ELEMENT_TYPE_LIST :
	          {
		     printf("LIST ELEMENT TYPE\n");
		     dumb_list_dump((Dumb_List *) list->elements[i].element, level + 1);
		  }
	          break;

	       case DUMB_LIST_ELEMENT_TYPE_HASH :
	          {
		     int lev;

                     lev = level + 1;
		     printf("HASH ELEMENT TYPE\n");
                     ecore_hash_for_each_node((Ecore_Hash *) list->elements[i].element, dumb_list_dump_each_hash_node, &lev);
		  }
	          break;

	       default :
	          {
		     printf("UNKNOWN ELEMENT TYPE!\n");
		  }
	          break;
	    }
      }
}

static void dumb_list_dump_each_hash_node(void *value, void *user_data)
{
   Ecore_Hash_Node *node;
   int level;
   int j;

   node = (Ecore_Hash_Node *) value;
   level = *((int *) user_data);
   for (j = 0; j < level; j++)
      printf(".");
   printf("%s = %s\n", (char *) node->key, (char *) node->value);
}

void
dumb_list_del(Dumb_List * list)
{
   int i;

   for (i = list->size - 1; i >= 0; i--)
      {
         if (list->elements[i].type == DUMB_LIST_ELEMENT_TYPE_LIST)
            dumb_list_del((Dumb_List *) list->elements[i].element);
         else if (list->elements[i].type == DUMB_LIST_ELEMENT_TYPE_HASH)
	   ecore_hash_destroy((Ecore_Hash *) list->elements[i].element);
      }

   E_FREE(list->elements);

   for (i = list->buffers_size - 1; i >= 0; i--)
      E_FREE(list->buffers[i]);
   E_FREE(list);
}
