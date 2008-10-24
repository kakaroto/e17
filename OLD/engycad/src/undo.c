/*
 * Copyright (C) 2002, Yury Hudobin
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

#include "engy.h"

typedef struct _Undo_Item_Ptr Undo_Item_Ptr;
typedef struct _Undo_Item_Int Undo_Item_Int;
typedef struct _Undo_Item_Double Undo_Item_Double;
typedef struct _Undo_Item_Obj Undo_Item_Obj;
typedef struct _Undo_Item Undo_Item;

struct _Undo_Item
{
    int                 type;
    int                 major;
    int                 minor;
    void               *obj;
};

struct _Undo_Item_Int
{
    int                 type;
    int                 major;
    int                 minor;
    void               *obj;
    void               *ptr;
    long                 old;
    long                 new;
};

struct _Undo_Item_Ptr
{
    int                 type;
    int                 major;
    int                 minor;
    void               *obj;
    void               *ptr;
    char               *old;
    char               *new;
};

struct _Undo_Item_Double
{
    int                 type;
    int                 major;
    int                 minor;
    void               *obj;
    void               *ptr;
    double              old;
    double              new;
};

struct _Undo_Item_Obj
{
    int                 type;
    int                 major;
    int                 minor;
    void               *obj;
    void               *ptr;
};

/* protos */
void                undo_ptr(Undo_Item_Ptr *);
void                undo_long(Undo_Item_Int *);
void                undo_double(Undo_Item_Double *);
void                undo_obj(Undo_Item_Obj *);

void                redo_ptr(Undo_Item_Ptr *);
void                redo_long(Undo_Item_Int *);
void                redo_double(Undo_Item_Double *);
void                redo_obj(Undo_Item_Obj *);

/* vars */
Eina_List          *undo_items = NULL;
Eina_List          *undo_pos = NULL;
Eina_List          *garbage = NULL;
Eina_List          *garbage_obj = NULL;

void
dump_list(void)
{
    Eina_List          *l;

    printf("Beg\n");
    printf("pos = %x\n", undo_pos);
    for (l = undo_items; l; l = l->next)
      {
          printf("> %x %x\n", l, l->data);
      }
    printf("End\n\n");
}

void
free_garbage(void)
{
    Eina_List          *l;

    for (l = garbage; l; l = l->next)
        FREE(l->data);
    garbage = eina_list_free(garbage);

    for (l = garbage_obj; l; l = l->next)
	  msg_create_and_send(CMD_DELETE, ((Object*)l->data)->type, l->data);
    garbage_obj = eina_list_free(garbage_obj);

}

void
cut_undo_list(void)
{
    Eina_List          *l;
    void               *p = NULL, *p1 = NULL;

    free_garbage();

    if (!undo_pos)
      {
          for (l = undo_items; l; l = l->next)
              FREE(l->data);
          undo_items = eina_list_free(undo_items);
          return;
      }

    if (undo_pos == eina_list_last(undo_items))
        return;

    p = undo_pos->data;
    do
      {
          p1 = eina_list_last(undo_items)->data;
          if (p != p1)
              undo_items = eina_list_remove(undo_items, p1);
      }
    while (p != p1);
};

void
append_undo_ptr(void *ptr,
                void *old, void *new, int major, int minor, void *obj)
{
    Undo_Item_Ptr      *it;

    it = (Undo_Item_Ptr *) malloc(sizeof(Undo_Item_Ptr));
    ENGY_ASSERT(it);
    memset(it, 0, sizeof(Undo_Item_Ptr));
    it->type = 11001;
    it->ptr = ptr;
    it->old = old;
    it->new = new;
    it->major = major;
    it->minor = minor;
    it->obj = obj;

    cut_undo_list();
    undo_items = eina_list_append(undo_items, it);
    undo_pos = eina_list_last(undo_items);
}

void
append_undo_long(void *ptr, long old, long new, int major, int minor, void *obj)
{
    Undo_Item_Int      *it;

    it = (Undo_Item_Int *) malloc(sizeof(Undo_Item_Int));
    ENGY_ASSERT(it);
    memset(it, 0, sizeof(Undo_Item_Int));
    it->type = 11002;
    it->ptr = ptr;
    it->old = old;
    it->new = new;
    it->major = major;
    it->minor = minor;
    it->obj = obj;

    cut_undo_list();
    undo_items = eina_list_append(undo_items, it);
    undo_pos = eina_list_last(undo_items);
}

void
append_undo_double(void *ptr,
                   double old, double new, int major, int minor, void *obj)
{
    Undo_Item_Double   *it;

    it = (Undo_Item_Double *) malloc(sizeof(Undo_Item_Double));
    ENGY_ASSERT(it);
    memset(it, 0, sizeof(Undo_Item_Double));
    it->type = 11003;
    it->ptr = ptr;
    it->old = old;
    it->new = new;
    it->major = major;
    it->minor = minor;
    it->obj = obj;

    cut_undo_list();

    undo_items = eina_list_append(undo_items, it);
    undo_pos = eina_list_last(undo_items);
}

void
append_undo_new_object(void *ptr, int major, int minor, void *obj)
{
    Undo_Item_Obj      *it;

    it = (Undo_Item_Obj *) malloc(sizeof(Undo_Item_Obj));
    ENGY_ASSERT(it);
    memset(it, 0, sizeof(Undo_Item_Obj));
    it->type = 11004;
    it->major = major;
    it->minor = minor;
    it->obj = obj;

    cut_undo_list();
    undo_items = eina_list_append(undo_items, it);
    undo_pos = eina_list_last(undo_items);
}

void
apply_undo_backward(void)
{
    Undo_Item          *it;

    if (!undo_items)
        return;
    if (!undo_pos)
        return;

    do
      {
          ENGY_ASSERT(undo_pos);
          it = (Undo_Item *) undo_pos->data;
          lock_data();
          switch (it->type)
            {
            case 11001:
                undo_ptr((Undo_Item_Ptr *) it);
                break;
            case 11002:
                undo_long((Undo_Item_Int *) it);
                break;
            case 11003:
                undo_double((Undo_Item_Double *) it);
                break;
            case 11004:
                undo_obj((Undo_Item_Obj *) it);
                break;
            default:
                ENGY_ASSERTS(0, "default");
            }
          unlock_data();

          undo_pos = undo_pos->prev;

      }
    while (!it->major);
}

void
apply_undo_forward(void)
{
    Undo_Item          *it;

    if (!undo_items)
        return;

    do
      {
          if (undo_pos && !undo_pos->next)
              return;

          if (undo_pos && undo_pos->next)
              undo_pos = undo_pos->next;

          if (!undo_pos)
              undo_pos = undo_items;

          it = (Undo_Item *) undo_pos->data;
          lock_data();
          switch (it->type)
            {
            case 11001:
                redo_ptr((Undo_Item_Ptr *) it);
                break;
            case 11002:
                redo_long((Undo_Item_Int *) it);
                break;
            case 11003:
                redo_double((Undo_Item_Double *) it);
                break;
            case 11004:
                redo_obj((Undo_Item_Obj *) it);
                break;
            default:
                ENGY_ASSERTS(0, "default");
            }
          unlock_data();
          if (!undo_pos->next)
              return;
          it = (Undo_Item *) undo_pos->next->data;
      }
    while (!it->major);
};

void
undo_ptr(Undo_Item_Ptr * it)
{
    char              **p;

    p = (char **)it->ptr;
    *p = it->old;
    garbage = eina_list_append(garbage, it->new);
    msg_create_and_send(it->major, it->minor, it->obj);
}

void
undo_long(Undo_Item_Int * it)
{
    long                *p;

    p = (long *)it->ptr;
    *p = it->old;
    msg_create_and_send(it->major, it->minor, it->obj);
}

void
undo_double(Undo_Item_Double * it)
{
    double             *p;

    p = (double *)it->ptr;
    *p = it->old;
    msg_create_and_send(it->major, it->minor, it->obj);
}

void
undo_obj(Undo_Item_Obj * it)
{
    Object             *o;

    o = it->obj;
    o->flags |= FLAG_DELETED;
    garbage_obj = eina_list_append(garbage_obj, o);
    msg_create_and_send(it->major, it->minor, it->obj);
}

void
redo_ptr(Undo_Item_Ptr * it)
{
    char              **p;

    p = (char **)it->ptr;
    *p = it->new;
    garbage = eina_list_remove(garbage, it->new);
    msg_create_and_send(it->major, it->minor, it->obj);
}

void
redo_long(Undo_Item_Int * it)
{
    long                *p;

    p = (long *)it->ptr;
    *p = it->new;
    msg_create_and_send(it->major, it->minor, it->obj);
}

void
redo_double(Undo_Item_Double * it)
{
    double             *p;

    p = (double *)it->ptr;
    *p = it->new;
    msg_create_and_send(it->major, it->minor, it->obj);
}

void
redo_obj(Undo_Item_Obj * it)
{
    Object             *o;

    o = it->obj;
    o->flags ^= FLAG_DELETED;
    garbage_obj = eina_list_remove(garbage_obj, o);
    msg_create_and_send(it->major, it->minor, it->obj);
}



