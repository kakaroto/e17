/* geist_list.c

Copyright (C) 1999,2000 Tom Gilbert.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies of the Software and its documentation and acknowledgment shall be
given in the documentation and software packages that this Software was
used.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#include "geist_list.h"

geist_list *
geist_list_new(void)
{
   geist_list *l;

   l = (geist_list *) malloc(sizeof(geist_list));
   l->data = NULL;
   l->next = NULL;
   l->prev = NULL;
   return l;
}

void
geist_list_free(geist_list * l)
{
   geist_list *ll;

   if (!l)
      return;

   while (l)
   {
      ll = l;
      l = l->next;
      free(ll);
   }
}

void
geist_list_free_and_data(geist_list * l)
{
   geist_list *ll;

   if (!l)
      return;

   while (l)
   {
      ll = l;
      l = l->next;
      free(ll->data);
      free(ll);
   }
}

geist_list *
geist_list_dup(geist_list * list)
{
   geist_list *ret = NULL;

   if (list)
   {
      geist_list *last;

      ret = geist_list_new();
      ret->data = list->data;
      last = ret;
      list = list->next;
      while (list)
      {
         last->next = geist_list_new();
         last->next->prev = last;
         last = last->next;
         last->data = list->data;
         list = list->next;
      }
   }

  return ret;
}

geist_list *
geist_list_dup_special(geist_list * list,
                       void (*cpy_func) (void **dest, void *data))
{
   geist_list *ret = NULL;

   

   if (list)
   {
      geist_list *last;

      ret = geist_list_new();
      cpy_func(&(ret->data), list->data);
      last = ret;
      list = list->next;
      while (list)
      {
         last->next = geist_list_new();
         last->next->prev = last;
         last = last->next;
         cpy_func(&(last->data), list->data);
         list = list->next;
      }
   }

   return ret;
}

geist_list *
geist_list_add_front(geist_list * root, void *data)
{
   geist_list *l;

   
   l = geist_list_new();
   l->next = root;
   l->prev = NULL;
   l->data = data;
   if (root)
      root->prev = l;
   return l;
}

geist_list *
geist_list_add_end(geist_list * root, void *data)
{
   geist_list *l, *last;

   
   last = geist_list_last(root);
   l = geist_list_new();
   l->next = NULL;
   l->prev = last;
   l->data = data;
   if (last)
   {
      last->next = l;
      return root;
   }
   else
   {
      return l;
   }
}

geist_list *
geist_list_add_at_pos(geist_list * root, int pos, void *data)
{
   geist_list *l, *top;

   

   if (pos == geist_list_length(root))
   {
      root = geist_list_add_end(root, data);
   }
   else if (pos == 0)
   {
      root = geist_list_add_front(root, data);
   }
   else
   {
      top = geist_list_nth(root, pos);

      if (!top)
         return root;

      l = geist_list_new();
      l->next = top;
      l->prev = top->prev;
      l->data = data;
      if (top->prev)
         top->prev->next = l;

      top->prev = l;
   }
   return root;
}

geist_list *
geist_list_move_up_by_one(geist_list * root, geist_list * l)
{
   
   if (l || l->prev)
      root = geist_list_move_down_by_one(root, l->prev);
   return root;
}

geist_list *
geist_list_move_down_by_one(geist_list * root, geist_list * l)
{
   geist_list *temp;

   if (!l || !l->next)
      return root;

   /* store item we link next to */
   temp = l->next;
   /* remove from list */
   root = geist_list_unlink(root, l);
   /* add back one before */
   l->next = temp->next;
   l->prev = temp;
   if (temp->next)
   {
      temp->next->prev = l;
   }
   temp->next = l;

   return root;
}


unsigned char
geist_list_has_more_than_one_item(geist_list * root)
{
   

   if (root->next)
   {
      return 1;
   }
   else
   {
      return 0;
   }
}

geist_list *
geist_list_pop_to_end(geist_list * root, geist_list * l)
{
   
   root = geist_list_unlink(root, l);

   root = geist_list_add_end(root, l->data);

   free(l);

   return root;
}

geist_list *
geist_list_cat(geist_list * root, geist_list * l)
{
   geist_list *last;

   
   if (!l)
      return root;
   if (!root)
      return l;
   last = geist_list_last(root);
   last->next = l;
   l->prev = last;
   return root;
}

int
geist_list_length(geist_list * l)
{
   int length;

   length = 0;
   while (l)
   {
      length++;
      l = l->next;
   }
   return length;
}

geist_list *
geist_list_last(geist_list * l)
{
   
   if (l)
   {
      while (l->next)
         l = l->next;
   }
   return l;
}

geist_list *
geist_list_first(geist_list * l)
{
   
   if (l)
   {
      while (l->prev)
         l = l->prev;
   }
   return l;
}

geist_list *
geist_list_jump(geist_list * root, geist_list * l, int direction, int num)
{
   int i;
   geist_list *ret = NULL;

   
   if (!root)
      return NULL;
   if (!l)
      return root;

   ret = l;

   for (i = 0; i < num; i++)
   {
      if (direction == FORWARD)
      {
         if (ret->next)
            ret = ret->next;
         else
            ret = root;
      }
      else
      {
         if (ret->prev)
            ret = ret->prev;
         else
            ret = geist_list_last(ret);
      }
   }
   return ret;
}

geist_list *
geist_list_reverse(geist_list * l)
{
   geist_list *last;

   
   last = NULL;
   while (l)
   {
      last = l;
      l = last->next;
      last->next = last->prev;
      last->prev = l;
   }

   return last;
}

geist_list *
geist_list_randomize(geist_list * list)
{
   int len, r, i;
   geist_list **farray, *f, *t;

   
   if (!list)
      return NULL;
   len = geist_list_length(list);
   if (len <= 1)
      return list;
   farray = (geist_list **) malloc(sizeof(geist_list *) * len);
   for (f = list, i = 0; f; f = f->next, i++)
   {
      farray[i] = f;
   }
   srand(getpid() * time(NULL) % ((unsigned int) -1));
   for (i = 0; i < len - 1; i++)
   {
      r = (int) ((len - i - 1) * ((float) rand()) / (RAND_MAX + 1.0)) + i + 1;
      if (i == r)
         abort();
      t = farray[i];
      farray[i] = farray[r];
      farray[r] = t;
   }
   list = farray[0];
   list->prev = NULL;
   list->next = farray[1];
   for (i = 1, f = farray[1]; i < len - 1; i++, f = f->next)
   {
      f->prev = farray[i - 1];
      f->next = farray[i + 1];
   }
   f->prev = farray[len - 2];
   f->next = NULL;
   free(farray);
   return list;
}

int
geist_list_num(geist_list * root, geist_list * l)
{
   int i = 0;

   while (root)
   {
      if (root == l)
         return i;
      i++;
      root = root->next;
   }
   return -1;
}

geist_list *
geist_list_unlink(geist_list * root, geist_list * l)
{
   

   if (!l)
      return root;

   if ((!root) || ((l == root) && (!l->next)))
      return NULL;

   if (l->prev)
      l->prev->next = l->next;
   if (l->next)
      l->next->prev = l->prev;
   if (root == l)
      root = root->next;
   return root;
}


geist_list *
geist_list_remove(geist_list * root, geist_list * l)
{
   
   root = geist_list_unlink(root, l);
   free(l);
   return root;
}

geist_list *
geist_list_sort(geist_list * list, geist_compare_fn cmp)
{
   geist_list *l1, *l2;

   

   if (!list)
      return NULL;
   if (!list->next)
      return list;

   l1 = list;
   l2 = list->next;

   while ((l2 = l2->next) != NULL)
   {
      if ((l2 = l2->next) == NULL)
         break;
      l1 = l1->next;
   }
   l2 = l1->next;
   l1->next = NULL;

   return
            geist_list_sort_merge(geist_list_sort(list, cmp),
                                  geist_list_sort(l2, cmp), cmp);
}

geist_list *
geist_list_sort_merge(geist_list * l1, geist_list * l2, geist_compare_fn cmp)
{
   geist_list list, *l, *lprev;

   

   l = &list;
   lprev = NULL;

   while (l1 && l2)
   {
      if (cmp(l1->data, l2->data) < 0)
      {
         l->next = l1;
         l = l->next;
         l->prev = lprev;
         lprev = l;
         l1 = l1->next;
      }
      else
      {
         l->next = l2;
         l = l->next;
         l->prev = lprev;
         lprev = l;
         l2 = l2->next;
      }
   }
   l->next = l1 ? l1 : l2;
   l->next->prev = l;

   return list.next;
}

geist_list *
geist_list_nth(geist_list * root, unsigned int num)
{
   unsigned int i;
   geist_list *l;

   

   if (num > (unsigned int) geist_list_length(root))
      return root;
   l = root;
   for (i = 0; l; ++i)
   {
      if (i == num)
         return l;
      l = l->next;
   }
   return root;
}
