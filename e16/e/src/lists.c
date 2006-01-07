/*
 * Copyright (C) 2000-2006 Carsten Haitzler, Geoff Harrison and various contributors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies of the Software, its documentation and marketing & publicity
 * materials, and acknowledgment shall be given in the documentation, materials
 * and software packages that this Software was used.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include "E.h"

typedef struct _list
{
   char               *name;
   int                 id;
   void               *item;

   struct _list       *next;
} List;

static List        *lists;

void
ListsInit(int num)
{
   lists = Ecalloc(num, sizeof(List));
}

void               *
FindItem(const void *name, int id, int find_by, int type)
{
   List               *ptr;

   ptr = lists[type].next;
   if (find_by == LIST_FINDBY_NAME)
     {
	if (!name)
	   return NULL;
	while (ptr)
	  {
	     if (!strcmp(name, ptr->name))
		return ptr->item;
	     ptr = ptr->next;
	  }
     }
   else if (find_by == LIST_FINDBY_ID)
     {
	while (ptr)
	  {
	     if ((ptr->id == id))
		return ptr->item;
	     ptr = ptr->next;
	  }
     }
   else if (find_by == LIST_FINDBY_BOTH)
     {
	if (!name)
	   return NULL;
	while (ptr)
	  {
	     if ((!strcmp(name, ptr->name)) && (ptr->id == id))
		return ptr->item;
	     ptr = ptr->next;
	  }
     }
   else if (find_by == LIST_FINDBY_POINTER)
     {
	while (ptr)
	  {
	     if (ptr->item == name)
		return ptr->item;
	     ptr = ptr->next;
	  }
     }
   else if (find_by == LIST_FINDBY_NONE)
     {
	while (ptr)
	  {
	     return ptr->item;
	  }
     }
   return NULL;
}

void
AddItem(const void *item, const char *name, int id, int type)
{
   List               *ptr;

   ptr = Emalloc(sizeof(List));
   if (!ptr)
      return;
   ptr->item = (void *)item;
   ptr->name = Estrdup(name);
   ptr->id = id;
   ptr->next = lists[type].next;
   lists[type].next = ptr;
}

void
AddItemEnd(const void *item, const char *name, int id, int type)
{
   List               *ptr, *p;

   ptr = Emalloc(sizeof(List));
   if (!ptr)
      return;
   ptr->item = (void *)item;
   ptr->name = Estrdup(name);
   ptr->id = id;
   ptr->next = NULL;
   p = lists[type].next;
   if (!p)
     {
	lists[type].next = ptr;
	return;
     }
   while (p)
     {
	if (!p->next)
	  {
	     p->next = ptr;
	     return;
	  }
	p = p->next;
     }
}

void
MoveItemToListTop(const void *item, int type)
{
   List               *ptr, *pptr;

   pptr = NULL;
   ptr = lists[type].next;
   while (ptr)
     {
	if (ptr->item == item)
	  {
	     if (pptr)
	       {
		  pptr->next = ptr->next;
		  ptr->next = lists[type].next;
		  lists[type].next = ptr;
		  return;
	       }
	     else
	       {
		  return;
	       }
	  }
	else
	  {
	     pptr = ptr;
	     ptr = ptr->next;
	  }
     }
}

#if 0				/* Unused */
void
MoveItemToListBottom(const void *item, int type)
{
   List               *ptr, *pptr, *nptr;

   pptr = lists + type;
   ptr = pptr->next;
   while (ptr)
     {
	if (ptr->item == item)
	  {
	     if (ptr->next)
	       {
		  nptr = ptr;
		  while (nptr->next)
		    {
		       nptr = nptr->next;
		    }
		  pptr->next = ptr->next;
		  nptr->next = ptr;
		  ptr->next = NULL;
		  return;
	       }
	     else
	       {
		  return;
	       }
	  }
	else
	  {
	     pptr = ptr;
	     ptr = ptr->next;
	  }
     }
}
#endif

void               *
RemoveItem(const void *name, int id, int find_by, int type)
{
   List               *ptr, *pptr;
   void               *p;

   pptr = NULL;
   ptr = lists[type].next;
   if (find_by == LIST_FINDBY_NAME)
     {
	while (ptr)
	  {
	     if ((!strcmp(name, ptr->name)))
	       {
		  if (pptr)
		     pptr->next = ptr->next;
		  else
		     lists[type].next = ptr->next;
		  p = ptr->item;
		  if (ptr->name)
		     Efree(ptr->name);
		  Efree(ptr);
		  return p;
	       }
	     pptr = ptr;
	     ptr = ptr->next;
	  }
     }
   else if (find_by == LIST_FINDBY_ID)
     {
	while (ptr)
	  {
	     if ((ptr->id == id))
	       {
		  if (pptr)
		     pptr->next = ptr->next;
		  else
		     lists[type].next = ptr->next;
		  p = ptr->item;
		  if (ptr->name)
		     Efree(ptr->name);
		  Efree(ptr);
		  return p;
	       }
	     pptr = ptr;
	     ptr = ptr->next;
	  }
     }
   else if (find_by == LIST_FINDBY_BOTH)
     {
	while (ptr)
	  {
	     if ((!strcmp(name, ptr->name)) && (ptr->id == id))
	       {
		  if (pptr)
		     pptr->next = ptr->next;
		  else
		     lists[type].next = ptr->next;
		  p = ptr->item;
		  if (ptr->name)
		     Efree(ptr->name);
		  Efree(ptr);
		  return p;
	       }
	     pptr = ptr;
	     ptr = ptr->next;
	  }
     }
   else if (find_by == LIST_FINDBY_POINTER)
     {
	while (ptr)
	  {
	     if (ptr->item == name)
	       {
		  if (pptr)
		     pptr->next = ptr->next;
		  else
		     lists[type].next = ptr->next;
		  p = ptr->item;
		  if (ptr->name)
		     Efree(ptr->name);
		  Efree(ptr);
		  return p;
	       }
	     pptr = ptr;
	     ptr = ptr->next;
	  }
     }
   else if (find_by == LIST_FINDBY_NONE)
     {
	while (ptr)
	  {
	     if (pptr)
		pptr->next = ptr->next;
	     else
		lists[type].next = ptr->next;
	     p = ptr->item;
	     if (ptr->name)
		Efree(ptr->name);
	     Efree(ptr);
	     return p;
#if 0				/* FIXME - Is something wrong here? */
	     pptr = ptr;
	     ptr = ptr->next;
#endif
	  }
     }

   return NULL;
}

void               *
RemoveItemByPtr(const void *ptritem, int type)
{
   List               *ptr, *pptr;
   void               *p;

   pptr = NULL;
   ptr = lists[type].next;

   while (ptr)
     {
	p = ptr->item;
	if (p == ptritem)
	  {
	     if (pptr)
	       {
		  pptr->next = ptr->next;
	       }
	     else
	       {
		  lists[type].next = ptr->next;
	       }
	     if (ptr->name)
		Efree(ptr->name);
	     Efree(ptr);
	     return p;
	  }
	pptr = ptr;
	ptr = ptr->next;
     }

   return NULL;
}

void              **
ListItemType(int *num, int type)
{
   List               *ptr;
   int                 i, len;
   void              **lst;

   *num = 0;
   len = 0;
   if (type == LIST_TYPE_ANY)
      return NULL;
   ptr = lists[type].next;
   while (ptr)
     {
	len++;
	ptr = ptr->next;
     }
   if (!len)
      return NULL;
   lst = Emalloc(len * sizeof(void *));

   i = 0;
   ptr = lists[type].next;
   while (ptr)
     {
	lst[i++] = ptr->item;
	ptr = ptr->next;
     }
   *num = i;
   return lst;
}

char              **
ListItems(int *num, int type)
{
   List               *ptr;
   int                 i, len;
   char              **list;

   i = 0;
   len = 0;
   list = NULL;
   ptr = lists[type].next;
   if (type != LIST_TYPE_ANY)
     {
	while (ptr)
	  {
	     len++;
	     ptr = ptr->next;
	  }
     }
   else
     {
	while (ptr)
	  {
	     len++;
	     ptr = ptr->next;
	  }
     }
   list = Emalloc(len * sizeof(char *));

   if (!list)
     {
	*num = 0;
	return NULL;
     }
   ptr = lists[type].next;
   if (type != LIST_TYPE_ANY)
     {
	while (ptr)
	  {
	     list[i] = Estrdup(ptr->name);
	     i++;
	     ptr = ptr->next;
	  }
     }
   else
     {
	while (ptr)
	  {
	     list[i] = Estrdup(ptr->name);
	     i++;
	     ptr = ptr->next;
	  }
     }
   *num = len;
   return list;
}

void              **
ListItemTypeID(int *num, int type, int id)
{
   List               *ptr;
   int                 i, len;
   void              **lst;

   *num = 0;
   if (type == LIST_TYPE_ANY)
      return NULL;
   len = 64;
   lst = Emalloc(len * sizeof(void *));

   i = 0;
   ptr = lists[type].next;
   while (ptr)
     {
	if ((ptr->id == id))
	   lst[i++] = ptr->item;
	if (i >= len)
	  {
	     len += 64;
	     lst = Erealloc(lst, len * sizeof(void *));
	  }
	ptr = ptr->next;
     }
   *num = i;
   if (i <= 0)
     {
	Efree(lst);
	return NULL;
     }
   return lst;
}

#if 0				/* Unused */
void              **
ListItemTypeName(int *num, int type, const char *name)
{
   List               *ptr;
   int                 i, len;
   void              **lst;

   *num = 0;
   if (type == LIST_TYPE_ANY)
      return NULL;
   len = 64;
   lst = Emalloc(len * sizeof(void *));

   i = 0;
   ptr = lists[type].next;
   while (ptr)
     {
	if ((!strcmp(name, ptr->name)))
	   lst[i++] = ptr->item;
	if (i >= len)
	  {
	     len += 64;
	     lst = Erealloc(lst, len * sizeof(void *));
	  }
	ptr = ptr->next;
     }
   *num = i;
   if (i <= 0)
     {
	Efree(lst);
	return NULL;
     }
   return lst;
}
#endif

void
ListChangeItemID(int type, void *item, int id)
{
   List               *ptr;

   ptr = lists[type].next;
   while (ptr)
     {
	if (ptr->item == item)
	  {
	     ptr->id = id;
	     return;
	  }
	ptr = ptr->next;
     }
}
