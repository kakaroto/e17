/* feh_list.c

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

#include "feh.h"
#include "feh_list.h"
#include "options.h"


feh_list *
feh_list_new(void)
{
   feh_list *l;

   D_ENTER(4);

   l = (feh_list *) emalloc(sizeof(feh_list));
   l->data = NULL;
   l->next = NULL;
   l->prev = NULL;
   D_RETURN(4,l);
}

feh_list *
feh_list_add_front(feh_list * root, void *data)
{
   feh_list *l;

   D_ENTER(4);
   l = feh_list_new();
   l->next = root;
   l->prev = NULL;
   l->data = data;
   if (root)
      root->prev = l;
   D_RETURN(4,l);
}

void
feh_list_free(feh_list * l)
{
   feh_list *ll;

   D_ENTER(4);

   if (!l)
      D_RETURN_(4);

   while (l)
   {
      ll = l;
      l = l->next;
      free(ll);
   }
   D_RETURN_(4);
}


feh_list *
feh_list_cat(feh_list * root, feh_list * l)
{
   feh_list *last;

   D_ENTER(4);
   if (!l)
      D_RETURN(4,root);
   if (!root)
      D_RETURN(4,l);
   last = feh_list_last(root);
   last->next = l;
   l->prev = last;
   D_RETURN(4,root);
}

int
feh_list_length(feh_list * l)
{
   int length;

   D_ENTER(4);
   length = 0;
   D(3,("list is %p\n", l));
   while (l)
   {
      length++;
      l = l->next;
   }
   D(3,("length is %d\n", length));
   D_RETURN(4,length);
}

feh_list *
feh_list_last(feh_list * l)
{
   D_ENTER(4);
   if (l)
   {
      while (l->next)
         l = l->next;
   }
   D_RETURN(4,l);
}

feh_list *
feh_list_first(feh_list * l)
{
   D_ENTER(4);
   if (l)
   {
      while (l->prev)
         l = l->prev;
   }
   D_RETURN(4,l);
}

feh_list *
feh_list_jump(feh_list * root, feh_list * l, int direction, int num)
{
   int i;
   feh_list *ret = NULL;

   D_ENTER(4);
   if (!root)
      D_RETURN(4,NULL);
   if (!l)
      D_RETURN(4,root);

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
            ret = feh_list_last(ret);
      }
   }
   D_RETURN(4,ret);
}

feh_list *
feh_list_reverse(feh_list * l)
{
   feh_list *last;

   D_ENTER(4);
   last = NULL;
   while (l)
   {
      last = l;
      l = last->next;
      last->next = last->prev;
      last->prev = l;
   }

   D_RETURN(4,last);
}

feh_list *
feh_list_randomize(feh_list * list)
{
   int len, r, i;
   feh_list **farray, *f, *t;

   D_ENTER(4);
   if (!list)
      D_RETURN(4,NULL);
   len = feh_list_length(list);
   if (len <= 1)
      D_RETURN(4,list);
   D(5,("List(%8p) has %d items.\n", list, len));
   farray = (feh_list **) malloc(sizeof(feh_list *) * len);
   for (f = list, i = 0; f; f = f->next, i++)
   {
      D(5,("filelist_randomize():  farray[%d] <- %8p\n", i, f));
      farray[i] = f;
   }
   srand(getpid() * time(NULL) % ((unsigned int) -1));
   for (i = 0; i < len - 1; i++)
   {
      r = (int) ((len - i - 1) * ((float) rand()) / (RAND_MAX + 1.0)) + i + 1;
      D(5,("i == %d, r == %d\n", i, r));
      if (i == r)
         abort();
      D(5,
        ("Swapping farray[%d] (%8p) with farray[%d] (%8p)\n", i, farray[i], r,
         farray[r]));
      t = farray[i];
      farray[i] = farray[r];
      farray[r] = t;
      D(5,("New values are %8p and %8p\n", farray[i], farray[r]));
   }
   list = farray[0];
   list->prev = NULL;
   list->next = farray[1];
   for (i = 1, f = farray[1]; i < len - 1; i++, f = f->next)
   {
      f->prev = farray[i - 1];
      f->next = farray[i + 1];
      D(5,
        ("Rebuilding list.  At farray[%d], f == %8p, f->prev == %8p, f->next == %8p\n",
         i, f, f->prev, f->next));
   }
   f->prev = farray[len - 2];
   f->next = NULL;
   free(farray);
   D_RETURN(4,list);
}

int
feh_list_num(feh_list * root, feh_list * l)
{
   int i = 0;

   D_ENTER(4);
   D(4,("list is %p\n", root));

   while (root)
   {
      if (root == l)
         D_RETURN(4,i);
      i++;
      root = root->next;
   }
   D_RETURN(4,-1);
}

feh_list *
feh_list_get_num(feh_list * root, unsigned int num)
{
   unsigned int i;
   feh_list * l;
   
   D_ENTER(4);

   if (num > (unsigned int)feh_list_length(root)) {
      weprintf("there aren't that many files in this list\n");
      D_RETURN(4,root);
   }
   l = root;
   for (i = 1; l; ++i )
   {
      if(i == num)
         D_RETURN(4,l);
      l = l->next;
   }
   D_RETURN(4,root);
}

      
feh_list *
feh_list_remove(feh_list * root, feh_list * l)
{
   D_ENTER(4);
   if (!l)
      D_RETURN(4,root);

   if ((!root) || ((l == root) && (!l->next)))
      D_RETURN(4,NULL);

   if (l->prev)
      l->prev->next = l->next;
   if (l->next)
      l->next->prev = l->prev;
   if (root == l)
      root = root->next;
   free(l);
   D(4,("returning list %p, list->next %p\n", root, root->next));
   D_RETURN(4,root);
}

feh_list *
feh_list_sort(feh_list * list, feh_compare_fn cmp)
{
   feh_list *l1, *l2;

   D_ENTER(4);

   if (!list)
      D_RETURN(4,NULL);
   if (!list->next)
      D_RETURN(4,list);

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

   D_RETURN(4,feh_list_sort_merge
            (feh_list_sort(list, cmp), feh_list_sort(l2, cmp), cmp));
}

feh_list *
feh_list_sort_merge(feh_list * l1, feh_list * l2, feh_compare_fn cmp)
{
   feh_list list, *l, *lprev;

   D_ENTER(4);

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

   D_RETURN(4,list.next);
}
