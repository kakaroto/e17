
/*
 *  Copyright (C) 2000-2004 Carsten Haitzler, Geoff Harrison and various contributors
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to
 *  deal in the Software without restriction, including without limitation the
 *  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 *  sell copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies of the Software, its documentation and marketing & publicity
 *  materials, and acknowledgment shall be given in the documentation, materials
 *  and software packages that this Software was used.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 *  THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 *  IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 *  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "E.h"

void               *
FindItem(const char *name, int id, int find_by, int type)
{
   List               *ptr;

   EDBUG(6, "FindItem");
   ptr = lists.next;
   if (find_by == LIST_FINDBY_NAME)
     {
	while (ptr)
	  {
	     if ((ptr->type == type) && (!strcmp(name, ptr->name)))
		EDBUG_RETURN(ptr->item);
	     ptr = ptr->next;
	  }
     }
   else if (find_by == LIST_FINDBY_ID)
     {
	while (ptr)
	  {
	     if ((ptr->type == type) && (ptr->id == id))
		EDBUG_RETURN(ptr->item);
	     ptr = ptr->next;
	  }
     }
   else if (find_by == LIST_FINDBY_BOTH)
     {
	while (ptr)
	  {
	     if ((ptr->type == type) && (!strcmp(name, ptr->name))
		 && (ptr->id == id))
		EDBUG_RETURN(ptr->item);
	     ptr = ptr->next;
	  }
     }
   else if (find_by == LIST_FINDBY_NONE)
     {
	while (ptr)
	  {
	     if ((ptr->type == type))
		EDBUG_RETURN(ptr->item);
	     ptr = ptr->next;
	  }
     }
   EDBUG_RETURN(NULL);
}

void
AddItem(void *item, const char *name, int id, int type)
{
   List               *ptr;

   EDBUG(6, "AddItem");
   ptr = Emalloc(sizeof(List));
   if (!ptr)
      EDBUG_RETURN_;
   ptr->item = item;
   ptr->name = Estrdup(name);
   ptr->id = id;
   ptr->type = type;
   ptr->next = lists.next;
   lists.next = ptr;
   EDBUG_RETURN_;
}

void               *
RemoveItem(char *name, int id, int find_by, int type)
{
   List               *ptr, *pptr;
   void               *p;

   EDBUG(6, "RemoveItem");
   pptr = NULL;
   ptr = lists.next;
   if (find_by == LIST_FINDBY_NAME)
     {
	while (ptr)
	  {
	     if ((ptr->type == type) && (!strcmp(name, ptr->name)))
	       {
		  if (pptr)
		     pptr->next = ptr->next;
		  else
		     lists.next = ptr->next;
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
	     if ((ptr->type == type) && (ptr->id == id))
	       {
		  if (pptr)
		     pptr->next = ptr->next;
		  else
		     lists.next = ptr->next;
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
	     if ((ptr->type == type) && (!strcmp(name, ptr->name))
		 && (ptr->id == id))
	       {
		  if (pptr)
		     pptr->next = ptr->next;
		  else
		     lists.next = ptr->next;
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
	     if ((ptr->type == type))
	       {
		  if (pptr)
		     pptr->next = ptr->next;
		  else
		     lists.next = ptr->next;
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
   ptr = lists.next;
   while (ptr)
     {
	if (ptr->type == type)
	   len++;
	ptr = ptr->next;
     }
   if (!len)
      EDBUG_RETURN(NULL);
   lst = Emalloc(len * sizeof(void *));

   i = 0;
   ptr = lists.next;
   while (ptr)
     {
	if (ptr->type == type)
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
   ptr = lists.next;
   if (type != LIST_TYPE_ANY)
     {
	while (ptr)
	  {
	     if (ptr->type == type)
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
   ptr = lists.next;
   if (type != LIST_TYPE_ANY)
     {
	while (ptr)
	  {
	     if (ptr->type == type)
	       {
		  list[i] = Estrdup(ptr->name);
		  i++;
	       }
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
   len = 0;
   if (type == LIST_TYPE_ANY)
      EDBUG_RETURN(NULL);
   ptr = lists.next;
   while (ptr)
     {
	if ((ptr->type == type) && (ptr->id == id))
	   len++;
	ptr = ptr->next;
     }
   if (!len)
      EDBUG_RETURN(NULL);
   lst = Emalloc(len * sizeof(void *));

   i = 0;
   ptr = lists.next;
   while (ptr)
     {
	if ((ptr->type == type) && (ptr->id == id))
	   lst[i++] = ptr->item;
	ptr = ptr->next;
     }
   *num = i;
   EDBUG_RETURN(lst);
}
