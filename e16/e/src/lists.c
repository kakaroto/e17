/*
 * Copyright (C) 2000 Carsten Haitzler, Geoff Harrison and various contributors
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

void               *
FindItem(char *name, int id, int find_by, int type)
{
   List               *ptr;

   EDBUG(6, "FindItem");
   ptr = lists[type].next;
   if (find_by == LIST_FINDBY_NAME)
     {
	while (ptr)
	  {
	     if ((!strcmp(name, ptr->name)))
		EDBUG_RETURN(ptr->item);
	     ptr = ptr->next;
	  }
     }
   else if (find_by == LIST_FINDBY_ID)
     {
	while (ptr)
	  {
	     if ((ptr->id == id))
		EDBUG_RETURN(ptr->item);
	     ptr = ptr->next;
	  }
     }
   else if (find_by == LIST_FINDBY_BOTH)
     {
	while (ptr)
	  {
	     if ((!strcmp(name, ptr->name)) && (ptr->id == id))
		EDBUG_RETURN(ptr->item);
	     ptr = ptr->next;
	  }
     }
   else if (find_by == LIST_FINDBY_POINTER)
     {
	while (ptr)
	  {
	     if (ptr->item == (void *)name)
		EDBUG_RETURN(ptr->item);
	     ptr = ptr->next;
	  }
     }
   else if (find_by == LIST_FINDBY_NONE)
     {
	while (ptr)
	  {
	     EDBUG_RETURN(ptr->item);
	  }
     }
   EDBUG_RETURN(NULL);
}

void
AddItem(void *item, char *name, int id, int type)
{
   List               *ptr;

   EDBUG(6, "AddItem");
   ptr = Emalloc(sizeof(List));
   if (!ptr)
      EDBUG_RETURN_;
   ptr->item = item;
   ptr->name = duplicate(name);
   ptr->id = id;
   ptr->next = lists[type].next;
   lists[type].next = ptr;
   EDBUG_RETURN_;
}

void
AddItemEnd(void *item, char *name, int id, int type)
{
   List               *ptr, *p;

   EDBUG(6, "AddItem");
   ptr = Emalloc(sizeof(List));
   if (!ptr)
      EDBUG_RETURN_;
   ptr->item = item;
   ptr->name = duplicate(name);
   ptr->id = id;
   ptr->next = NULL;
   p = lists[type].next;
   if (!p)
     {
	lists[type].next = ptr;
	EDBUG_RETURN_;
     }
   while (p)
     {
	if (!p->next)
	  {
	     p->next = ptr;
	     EDBUG_RETURN_;
	  }
	p = p->next;
     }
   EDBUG_RETURN_;
}

void
MoveItemToListTop(void *item, int type)
{
   List               *ptr, *pptr;

   EDBUG(6, "MoveItemToListTop");

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
		  EDBUG_RETURN_;
	       }
	     else
	       {
		  EDBUG_RETURN_;
	       }
	  }
	else
	  {
	     pptr = ptr;
	     ptr = ptr->next;
	  }
     }
   EDBUG_RETURN_;
}

void
MoveItemToListBottom(void *item, int type)
{
   List               *ptr, *pptr, *nptr;

   EDBUG(6, "MoveItemToListBottom");

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
		  EDBUG_RETURN_;
	       }
	     else
	       {
		  EDBUG_RETURN_;
	       }
	  }
	else
	  {
	     pptr = ptr;
	     ptr = ptr->next;
	  }
     }
   EDBUG_RETURN_;
}

void               *
RemoveItem(char *name, int id, int find_by, int type)
{
   List               *ptr, *pptr;
   void               *p;

   EDBUG(6, "RemoveItem");
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
		  EDBUG_RETURN(p);
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
		  EDBUG_RETURN(p);
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
		  EDBUG_RETURN(p);
	       }
	     pptr = ptr;
	     ptr = ptr->next;
	  }
     }
   else if (find_by == LIST_FINDBY_POINTER)
     {
	while (ptr)
	  {
	     if (ptr->item == (void *)name)
	       {
		  if (pptr)
		     pptr->next = ptr->next;
		  else
		     lists[type].next = ptr->next;
		  p = ptr->item;
		  if (ptr->name)
		     Efree(ptr->name);
		  Efree(ptr);
		  EDBUG_RETURN(p);
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
	     EDBUG_RETURN(p);
	     pptr = ptr;
	     ptr = ptr->next;
	  }
     }

   EDBUG_RETURN(NULL);
}

void               *
RemoveItemByPtr(void *ptritem, int type)
{
   List               *ptr, *pptr;
   void               *p;

   EDBUG(6, "RemoveItemByPtr");

   pptr = NULL;
   ptr = lists[type].next;

   while (ptr)
     {
	if (ptr == ptritem)
	  {
	     if (pptr)
	       {
		  pptr->next = ptr->next;
	       }
	     else
	       {
		  lists[type].next = ptr->next;
	       }
	     p = ptr->item;
	     if (ptr->name)
		Efree(ptr->name);
	     Efree(ptr);
	     EDBUG_RETURN(p);
	  }
	pptr = ptr;
	ptr = ptr->next;
     }

   EDBUG_RETURN(NULL);
}

void              **
ListItemType(int *num, int type)
{
   List               *ptr;
   int                 i, len;
   void              **lst;

   EDBUG(6, "ListItemType");
   *num = 0;
   len = 0;
   if (type == LIST_TYPE_ANY)
      EDBUG_RETURN(NULL);
   ptr = lists[type].next;
   while (ptr)
     {
	len++;
	ptr = ptr->next;
     }
   if (!len)
      EDBUG_RETURN(NULL);
   lst = Emalloc(len * sizeof(void *));

   i = 0;
   ptr = lists[type].next;
   while (ptr)
     {
	lst[i++] = ptr->item;
	ptr = ptr->next;
     }
   *num = i;
   EDBUG_RETURN(lst);
}

char              **
ListItems(int *num, int type)
{
   List               *ptr;
   int                 i, len;
   char              **list;

   EDBUG(7, "ListItems");
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
	EDBUG_RETURN(NULL);
     }
   ptr = lists[type].next;
   if (type != LIST_TYPE_ANY)
     {
	while (ptr)
	  {
	     list[i] = duplicate(ptr->name);
	     i++;
	     ptr = ptr->next;
	  }
     }
   else
     {
	while (ptr)
	  {
	     list[i] = duplicate(ptr->name);
	     i++;
	     ptr = ptr->next;
	  }
     }
   *num = len;
   EDBUG_RETURN(list);
}

void              **
ListItemTypeID(int *num, int type, int id)
{
   List               *ptr;
   int                 i, len;
   void              **lst;

   EDBUG(6, "ListItemType");
   *num = 0;
   if (type == LIST_TYPE_ANY)
      EDBUG_RETURN(NULL);
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
	EDBUG_RETURN(NULL);
     }
   EDBUG_RETURN(lst);
}

void              **
ListItemTypeName(int *num, int type, char *name)
{
   List               *ptr;
   int                 i, len;
   void              **lst;

   EDBUG(6, "ListItemTypeName");
   *num = 0;
   if (type == LIST_TYPE_ANY)
      EDBUG_RETURN(NULL);
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
	EDBUG_RETURN(NULL);
     }
   EDBUG_RETURN(lst);
}

void
ListChangeItemID(int type, void *item, int id)
{
   List               *ptr;

   EDBUG(6, "ListChangeItemID");
   ptr = lists[type].next;
   while (ptr)
     {
	if (ptr->item == item)
	  {
	     ptr->id = id;
	     EDBUG_RETURN_;
	  }
	ptr = ptr->next;
     }
   EDBUG_RETURN_;
}
