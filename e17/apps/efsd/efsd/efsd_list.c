/*

Copyright (C) 2000, 2001 Christian Kreibich <kreibich@aciri.org>.

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
#if HAVE_CONFIG_H
# include <config.h>
#endif
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <efsd_debug.h>
#include <efsd_misc.h>
#include <efsd_list.h>

struct efsd_list
{
  struct efsd_list *prev;
  struct efsd_list *next;
  void             *data;
};


EfsdList *
efsd_list_new(void *data)
{
  EfsdList *l;

  D_ENTER;

  l = NEW(EfsdList);
  l->prev = l->next = NULL;
  l->data = data;

  D_RETURN_(l);
}

EfsdList *
efsd_list_head(EfsdList *l)
{
  D_ENTER;                      

  if (!l)
    D_RETURN_(NULL);

  while (l->prev)
    l = l->prev;
  
  D_RETURN_(l);
}


EfsdList *
efsd_list_next(EfsdList *l)
{
  D_ENTER;

  if (!l)
    D_RETURN_(NULL);

  D_RETURN_(l->next);
}


EfsdList *
efsd_list_prev(EfsdList *l)
{
  D_ENTER;

  if (!l)
    D_RETURN_(NULL);

  D_RETURN_(l->prev);
}


EfsdList *
efsd_list_prepend(EfsdList *l, void *data)
{
  EfsdList *lnew;

  D_ENTER;
  lnew = efsd_list_new(data);
  lnew->next = l;

  if (l)
    l->prev = lnew;

  D_RETURN_(lnew);
}


void      
efsd_list_free(EfsdList *l, EfsdFunc free_func)
{
  EfsdList *lnext;

  D_ENTER;

  if (!l)
    D_RETURN;

  while (l)
    {
      lnext = l->next;
      free_func(l->data);
      free(l);
      l = lnext;
    }

  D_RETURN;
}


EfsdList *
efsd_list_remove(EfsdList *l, EfsdList *item, EfsdFunc free_func)
{
  EfsdList *prev;
  EfsdList *next;

  D_ENTER;
  if (!l)
    D_RETURN_(NULL);

  prev = item->prev;
  next = item->next;

  free_func(item->data);
  free(item);

  /* first item */
  if (!prev)
    {
      if (!next)
	{
	  D_RETURN_(NULL);
	}
      else
	{
	  next->prev = NULL;
	  D_RETURN_(next);
	}
    }

  /* last item */
  if (!next)
    {
      if (!prev)
	{
	  D_RETURN_(NULL);
	}
      else
	{
	  prev->next = NULL;
	  D_RETURN_(l);
	}      
    }

  /* middle item */
  prev->next = next;
  next->prev = prev;

  D_RETURN_(l);
}


void     *
efsd_list_data(EfsdList *l)
{
  D_ENTER;

  if (!l)
    D_RETURN_(NULL);

  D_RETURN_(l->data);
}
