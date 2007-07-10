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

#define AC_EXEC 0
#define AC_VMENU 1
#define AC_HMENU 2
#define AC_SEP 3
#define AC_NONE 4

typedef struct _Menu_Item
{
    char               *label;
    char               *param;
    char               *owner;
    char               *pic;
    int                 action_class;
    int                 level;
    double              x, y, w, h;
    Evas_Object        *ot;
    Evas_Object        *on;
    Evas_Object        *oh;
    Evas_Object        *oa;
    Evas_Object        *om;
    Evas_Object        *op;
}
Menu_Item;

typedef struct _Box
{
    char               *name;
    int                 level;
    Evas_List          *items;
    double              x, y;
    double              w, h;
}
Box;

typedef struct _Menu_Params
{
    char               *font;
    int                 size;
    int                 state;
    Evas_Object        *bg;
}
Menu_Params;

/* vars */

Evas_List          *top = NULL;
Evas_List          *boxes = NULL;
Evas_List          *items = NULL;
Menu_Params         mp;

/* proto */
/* TODO*/
void                menu_load(void);
void                menu_show(void);
void                menu_box_show_top(void);
void                menu_box_show(char *, double, double, int);
void                menu_box_hide(char *);
Menu_Item          *_menu_load_item(int, int);

void                menu_item_create_evas_object(Menu_Item *);

static void 
  _menu_mouse_down(void *data, Evas *_e, Evas_Object *_o, void *event_info);

static void 
    _menu_mouse_up(void *data, Evas *_e, Evas_Object *_o, void *event_info);

static void 
    _menu_mouse_in(void *data, Evas *_e, Evas_Object *_o, void *event_info);

static void 
  _menu_mouse_out(void *data, Evas *_e, Evas_Object *_o, void *event_info);

void
menu_init(void)
{
    mp.font = "lucon";
    mp.size = 10;
    mp.state = 0;
    mp.bg = evas_object_rectangle_add(shell->evas);
    evas_object_layer_set(mp.bg, 20);
    evas_object_color_set(mp.bg, 
		    200*ALPHA2/255, 
		    200*ALPHA2/255, 
		    ALPHA2, ALPHA2);
    menu_load();
    menu_show();
}

void
menu_item_create_evas_object(Menu_Item * mi)
{
    Evas               *e;
    char               *s1;
    char               *s;
    int                 res;

    e = shell->evas;
    s1 = my_iconv(shell->dcd, _(mi->label));

    mi->om = evas_object_rectangle_add(e);

    E_DB_STR_GET(shell->menu_file, "/normal", s, res);
    ENGY_ASSERT(res);
    if( s[0] != '/' ){
	char buf[4096];
	snprintf(buf, 4096,"%s/%s", shell->home,s); 
	mi->on = evas_image_load(buf);
	FREE(s);                        
    } else {                                            
	mi->on = evas_image_load(s);
	FREE(s);                                                
    }    
    
    E_DB_STR_GET(shell->menu_file, "/hilighted", s, res);
    ENGY_ASSERT(res);
    if( s[0] != '/' ){
	char buf[4096];
	snprintf(buf, 4096,"%s/%s", shell->home,s); 
	FREE(s);                            
	mi->oh = evas_image_load(buf);
    } else {                                            
	mi->oh = evas_image_load(s);
	FREE(s);                                                
    } 
    
    E_DB_STR_GET(shell->menu_file, "/active", s, res);
    ENGY_ASSERT(res);
    if( s[0] != '/' ){
	char buf[4096];
	snprintf(buf, 4096,"%s/%s", shell->home,s); 
	mi->oa = evas_image_load(buf);
	FREE(s);                            
    } else {                                            
	mi->oa = evas_image_load(s);
	FREE(s);                                                
    } 

    if (mi->pic)
    {
        mi->op = evas_image_load(mi->pic);
    }
    

    mi->ot = evas_object_text_add(e);
    evas_object_text_font_set(mi->ot, mp.font, mp.size);
    evas_object_text_text_set(mi->ot, s1);
    evas_object_layer_set(mi->ot, 20);
    evas_object_color_set(mi->ot, 0, 0, 0, 255);
    evas_object_pass_events_set(mi->ot, 1);

    evas_object_pass_events_set(mi->on, 1);
    evas_object_pass_events_set(mi->oh, 1);
    evas_object_pass_events_set(mi->oa, 1);
    evas_object_pass_events_set(mi->op, 1);
    evas_object_image_border_set(mi->on, 2, 2, 2, 2);
    evas_object_image_border_set(mi->oh, 2, 2, 2, 2);
    evas_object_image_border_set(mi->oa, 2, 2, 2, 2);

    evas_object_color_set(mi->om, 0, 0, 0, 0);
    evas_object_layer_set(mi->om, 20);
    evas_object_layer_set(mi->on, 20);
    evas_object_layer_set(mi->oh, 20);
    evas_object_layer_set(mi->oa, 20);
    evas_object_layer_set(mi->op, 20);
    evas_object_stack_above(mi->oa, mi->oh);
    evas_object_stack_above(mi->ot, mi->oa);

    evas_object_event_callback_add(mi->om, EVAS_CALLBACK_MOUSE_DOWN, _menu_mouse_down, NULL);
    evas_object_event_callback_add(mi->om, EVAS_CALLBACK_MOUSE_UP, _menu_mouse_up, NULL);
    evas_object_event_callback_add(mi->om, EVAS_CALLBACK_MOUSE_IN, _menu_mouse_in, NULL);
    evas_object_event_callback_add(mi->om, EVAS_CALLBACK_MOUSE_OUT, _menu_mouse_out, NULL);

/*    evas_set_clip(e, mi->ot, mi->oc);
    evas_set_clip(e, mi->om, mi->oc);
    evas_set_clip(e, mi->on, mi->oc);
    evas_set_clip(e, mi->oh, mi->oc);
    evas_set_clip(e, mi->oa, mi->oc);
    evas_object_show(mi->ot);*/
    FREE(s1);
    ENGY_ASSERTS(mi->ot, "menu item (text)");
    ENGY_ASSERTS(mi->om, "menu item (mouse rect)");
    ENGY_ASSERTS(mi->on, "menu item (normal)");
    ENGY_ASSERTS(mi->oh, "menu item (hilighted)");
    ENGY_ASSERTS(mi->on, "menu item (active)");
    ENGY_ASSERTS(mi->on, "menu item (pic)");
}

Menu_Item          *
_menu_load_item(int box, int item)
{
    Menu_Item          *mi;
    char                buf[4096];
    char               *s;
    int                 i, res;

    mi = (Menu_Item *) malloc(sizeof(Menu_Item));
    ENGY_ASSERT(mi);
    memset(mi, 0, sizeof(Menu_Item));

    sprintf(buf, "/menu/box/%d/%d/label", box, item);
    E_DB_STR_GET(shell->menu_file, buf, s, res);
    ENGY_ASSERT(res);
    if(s && (strlen(s)>4000)) s[4000]=0;
    mi->label = s;

    sprintf(buf, "/menu/box/%d/%d/action_class", box, item);
    E_DB_INT_GET(shell->menu_file, buf, i, res);
    ENGY_ASSERT(res);
    mi->action_class = i;
    if (i == AC_SEP)
      {
          FREE(mi->label);
          mi->label = DUP("");
      }

    sprintf(buf, "/menu/box/%d/%d/param", box, item);
    E_DB_STR_GET(shell->menu_file, buf, s, res);
    ENGY_ASSERT(res);
    if(s && (strlen(s)>4000)) s[4000]=0;
    mi->param = s;

    sprintf(buf, "/menu/box/%d/%d/pic", box, item);
    E_DB_STR_GET(shell->menu_file, buf, s, res);
    ENGY_ASSERT(res);
    if(s && (strlen(s)>4000)) s[4000]=0;
    if (strcmp(s, "NULL"))
        mi->pic = s;
    else 
	    free(s);

    menu_item_create_evas_object(mi);

    items = evas_list_append(items, mi);
    return mi;
}

void
menu_load(void)
{
    Evas_List          *l;
    char               *s;
    int                 i, n, res;

    E_DB_INT_GET(shell->menu_file, "/menu/box/count", n, res);
    ENGY_ASSERTS(res, "menu load");

    for (i = 0; i < n; i++)
      {
          char                buf[4096];
          Box                *box;
          int                 j, k;

          box = (Box *) malloc(sizeof(Box));
          ENGY_ASSERT(box);
          memset(box, 0, sizeof(Box));

          sprintf(buf, "/menu/box/%d/name", i);
	  E_DB_STR_GET(shell->menu_file, buf, s, res);
	  ENGY_ASSERT(res);
	  if(s && (strlen(s)>4000)) s[4000]=0;
	  box->name = s;
	  
	  sprintf(buf, "/menu/box/%d/count", i);
	  E_DB_INT_GET(shell->menu_file, buf, k, res);
	  ENGY_ASSERT(res);
          for (j = 0; j < k; j++)
            {
                Menu_Item          *mi;

                mi = _menu_load_item(i, j);
                mi->owner = box->name;
                box->items = evas_list_append(box->items, mi);
            }
          boxes = evas_list_append(boxes, box);
      }
}

void
_menu_item_move_resize(Menu_Item * mi,
                       double x, double y, double w, double h, int indent)
{
    Evas               *e;
    double              h1, h2;
    double              w1 = 0;

    e = shell->evas;
    h1 = evas_object_text_max_ascent_get(mi->ot);
    h2 = evas_object_text_max_descent_get(mi->ot);
    h1 = h1 - h2;

    if (indent)
        w1 = 18.0;
    w += w1;

    mi->x = x;
    mi->y = y;
    mi->w = w;
    mi->h = h;

    evas_object_move(mi->op, x, y);

    evas_object_hide(mi->ot);
    if (h > 1)
        evas_object_show(mi->ot);

    evas_object_move(mi->ot, x + w1, y + 1);
    evas_object_move(mi->om, x, y);
    evas_object_move(mi->on, x, y);
    evas_object_move(mi->oh, x, y);
    evas_object_move(mi->oa, x, y);

    evas_object_resize(mi->om, w, h);
    evas_object_resize(mi->on, w, h);
    evas_object_image_fill_set(mi->on, 0, 0, w, h);
    evas_object_resize(mi->oh, w, h);
    evas_object_image_fill_set(mi->oh, 0, 0, w, h);
    evas_object_resize(mi->oa, w, h);
    evas_object_image_fill_set(mi->oa, 0, 0, w, h);
    evas_object_resize(mi->op, h, h);
    evas_object_image_fill_set(mi->op, 0, 0, h, h);
}

void
menu_show(void)
{
    Evas               *e;
    Evas_List          *l;
    double              x = 0.0;
    double              h;

    e = shell->evas;

    evas_object_resize(mp.bg, 10000, 18);
    evas_object_show(mp.bg);
    menu_box_show_top();
}

/* menu_item */
void
_menu_item_normal(Menu_Item * mi)
{
    Evas               *e;

    e = shell->evas;
    if (mi->action_class == AC_NONE)
      {
          evas_object_hide(mi->ot);
      }
    else
      {
          evas_object_show(mi->ot);
      }
    evas_object_color_set(mi->ot, 0, 0, 0, ALPHA2);
    evas_object_color_set(mi->op, ALPHA2, ALPHA2, ALPHA2, ALPHA2);
    evas_object_show(mi->on);
    evas_object_show(mi->om);
    evas_object_show(mi->op);
    evas_object_hide(mi->oh);
    evas_object_hide(mi->oa);
}

void
_menu_item_hilight(Menu_Item * mi)
{
    Evas_List          *l;
    Evas               *e;
    Box                *box;

    e = shell->evas;

    for (l = boxes; l; l = l->next)
      {
          Box                *b;

          b = (Box *) l->data;
          if (b->level > mi->level)
              menu_box_hide(DUP(b->name));
          if (!strcmp(mi->owner, b->name))
              box = b;
      }
    for (l = box->items; l; l = l->next)
      {
          Menu_Item          *m;

          m = (Menu_Item *) l->data;
          _menu_item_normal(m);
      }

    evas_object_color_set(mi->ot, 0, 0, 0, ALPHA4);
    evas_object_color_set(mi->op, ALPHA4, ALPHA4, ALPHA4, ALPHA4);
    evas_object_show(mi->ot);
    evas_object_show(mi->oh);
    evas_object_show(mi->om);
    evas_object_show(mi->op);
    evas_object_hide(mi->on);
    evas_object_hide(mi->oa);

    if (mi->action_class == AC_NONE)
      {
          evas_object_hide(mi->ot);
      }
    else
      {
          evas_object_show(mi->ot);
      }
}

void
_menu_item_active(Menu_Item * mi)
{
    Evas_List          *l;
    Evas               *e;
    Box                *box;

    e = shell->evas;

    for (l = boxes; l; l = l->next)
      {
          Box                *b;

          b = (Box *) l->data;
          if (b->level > mi->level)
              menu_box_hide(DUP(b->name));
          if (!strcmp(mi->owner, b->name))
              box = b;
      }
    for (l = box->items; l; l = l->next)
      {
          Menu_Item          *m;

          m = (Menu_Item *) l->data;
          _menu_item_normal(m);
      }

    evas_object_color_set(mi->ot, 0, 0, 0, ALPHA4);
    evas_object_color_set(mi->op, 255, 255, 255, ALPHA4);
    evas_object_show(mi->ot);
    evas_object_show(mi->oa);
    evas_object_show(mi->om);
    evas_object_show(mi->op);
    evas_object_hide(mi->on);
    evas_object_hide(mi->oh);

    if (mi->action_class == AC_NONE)
      {
          evas_object_hide(mi->ot);
      }
    else
      {
          evas_object_show(mi->ot);
      }
}

void
_menu_item_hide(Menu_Item * mi)
{
    Evas               *e;

    e = shell->evas;

    evas_object_hide(mi->ot);
    evas_object_hide(mi->oa);
    evas_object_hide(mi->om);
    evas_object_hide(mi->op);
    evas_object_hide(mi->on);
    evas_object_hide(mi->oh);
}

/* menu_box */

void
menu_box_show_top(void)
{
    Evas               *e;
    Evas_List          *l;
    Box                *top = NULL;
    Evas_Coord          x, y, w, h;

    e = shell->evas;
    evas_output_viewport_get(e, &x, &y, &w, &h);
    for (l = boxes; l; l = l->next)
      {
          Box                *b;

          b = (Box *) l->data;
          if (!strcmp(b->name, "top"))
              top = b;
          if (b->level > 0)
              menu_box_hide(DUP(b->name));
      }

    top->level = 0;
    for (l = top->items; l; l = l->next)
      {
          Menu_Item          *mi;

          mi = (Menu_Item *) l->data;
          w = evas_object_text_horiz_advance_get(mi->ot);
          _menu_item_move_resize(mi, x, y, w, 18, 0);
          _menu_item_normal(mi);
          mi->level = 0;
          x += w;
      }
}

void
menu_box_hide_all(void)
{

}

void
menu_box_show(char *name, double x, double y, int level)
{
    Evas               *e;
    Evas_List          *l;
    Box                *box = NULL;
    double              w, h;
    int                 indent = 0;

    e = shell->evas;
    for (l = boxes; l; l = l->next)
      {
          Box                *b;

          b = (Box *) l->data;
          if (!strcmp(b->name, name))
              box = b;
          if (b->level >= level)
              menu_box_hide(DUP(b->name));
      }

    h = 18;
    w = 0;
    box->level = level;
    for (l = box->items; l; l = l->next)
      {
          Menu_Item          *mi;
          double              w1;

          mi = (Menu_Item *) l->data;
	  w1 = evas_object_text_horiz_advance_get(mi->ot);
          w = (w > w1) ? w : w1;
          /*if(mi->op) */ indent = 1;
      }

    for (l = box->items; l; l = l->next)
      {
          Menu_Item          *mi;
          double              h1;

          mi = (Menu_Item *) l->data;
          if (mi->action_class == AC_SEP)
            {
                h1 = 6;
            }
          else
            {
                h1 = h;
            };
          if (mi->action_class == AC_NONE)
              h1 = 0;
          _menu_item_move_resize(mi, x, y, w, h1, indent);
          _menu_item_normal(mi);
          mi->level = level;
          y += h1;
      }
    FREE(name);
}

void
menu_box_hide(char *name)
{
    Evas               *e;
    Evas_List          *l;
    Box                *box = NULL;
    double              w, h;

    e = shell->evas;
    for (l = boxes; l && (!box); l = l->next)
      {
          Box                *b;

          b = (Box *) l->data;
          if (!strcmp(b->name, name))
              box = b;
      }

    for (l = box->items; l; l = l->next)
      {
          Menu_Item          *mi;

          mi = (Menu_Item *) l->data;
          _menu_item_hide(mi);
      }
    FREE(name);
}

/* menu callbacks */
static void
_menu_mouse_down(void *data, Evas *_e, Evas_Object *_o, void *event_info)
{
    Evas_List          *l;
    Menu_Item          *mi = NULL;
    int                 ac;

    for (l = items; l && (!mi); l = l->next)
      {
          Menu_Item          *m;

          m = (Menu_Item *) l->data;
          if (m->om == _o)
              mi = m;
      }
    ac = mi->action_class;

    if (ac == AC_EXEC)
        _menu_item_active(mi);

    if ((ac == AC_VMENU) && (mp.state))
      {
          menu_box_show_top();
          mp.state = 0;
          _menu_item_hilight(mi);
          return;
      }

    if ((ac == AC_VMENU) && (!mp.state))
      {
          mp.state = 1;
          _menu_item_active(mi);
          menu_box_show(DUP(mi->param), mi->x, mi->y + mi->h, mi->level + 1);
          return;
      }
}

static void
_menu_mouse_up(void *data, Evas *_e, Evas_Object *_o, void *event_info)
{
    Evas_List          *l;
    Menu_Item          *mi = NULL;
    int                 ac;

    pointer_pop(0x1100 + POINTER_ARROW);
    for (l = items; l && (!mi); l = l->next)
      {
          Menu_Item          *m;

          m = (Menu_Item *) l->data;
          if (m->om == _o)
              mi = m;
      }
    ac = mi->action_class;

    if (ac == AC_EXEC)
      {
          gui_put_string(DUP(mi->param));
          mp.state = 2;
          menu_box_show_top();
      }
}

static void
_menu_mouse_in(void *data, Evas *_e, Evas_Object *_o, void *event_info)
{
    Evas_List          *l;
    Menu_Item          *mi = NULL;
    int                 ac;

    pointer_push_and_set(0x1100 + POINTER_ARROW);
    for (l = items; l && (!mi); l = l->next)
      {
          Menu_Item          *m;

          m = (Menu_Item *) l->data;
          if (m->om == _o)
              mi = m;
      }
    ac = mi->action_class;

    if (ac == AC_EXEC)
        _menu_item_hilight(mi);

    if ((ac == AC_VMENU) && (mp.state))
      {
          _menu_item_active(mi);
          menu_box_show(DUP(mi->param), mi->x, mi->y + mi->h, mi->level + 1);
          return;
      }

    if ((ac == AC_VMENU) && (!mp.state))
        _menu_item_hilight(mi);

    if (ac == AC_HMENU)
      {
          _menu_item_active(mi);
          menu_box_show(DUP(mi->param), mi->x + mi->w, mi->y, mi->level + 1);
          return;
      }

}

static void
_menu_mouse_out(void *data, Evas *_e, Evas_Object *_o, void *event_info)
{
    Evas_List          *l;
    Menu_Item          *mi = NULL;
    int                 ac;

    pointer_pop(0x1100 + POINTER_ARROW);
    for (l = items; l && (!mi); l = l->next)
      {
          Menu_Item          *m;

          m = (Menu_Item *) l->data;
          if (m->om == _o)
              mi = m;
      }
    ac = mi->action_class;
    if ((ac == AC_VMENU) && (!mp.state))
      {
          _menu_item_normal(mi);
          menu_box_hide(DUP(mi->param));
          return;
      }
    if ((ac == AC_VMENU) && (mp.state))
      {
/*	_menu_item_normal(mi);*/
/*	menu_box_hide(DUP(mi->param));*/
      }
//    if(ac==AC_HMENU)_menu_item_normal(mi);
    if ((ac == AC_EXEC) && (mp.state != 2))
        _menu_item_normal(mi);
    else
	mp.state = 0;

}

void
menu_item_reset(char *id, char *label, int ac, char *image, char *param)
{
    Evas_List          *l;
    Evas               *e;
    Menu_Item          *mi, *nmi = NULL;
    char               *s;

    e = shell->evas;

    for (l = items; l && !nmi; l = l->next)
      {
          mi = (Menu_Item *) l->data;
          if (!strcmp(mi->label, id))
              nmi = mi;
      }

    ENGY_ASSERTS(nmi, "menu item reset");

    s = my_iconv(shell->dcd, _(label));
    evas_object_text_text_set(nmi->ot, s);
    FREE(s);

    nmi->action_class = ac;

    IF_FREE(nmi->pic);
    nmi->pic = image;
    evas_object_del(nmi->op);
    if (nmi->pic)
      {
          nmi->op = evas_image_load(image);
          evas_object_pass_events_set(mi->op, 1);
          evas_object_layer_set(mi->op, 20);
      }

    IF_FREE(nmi->param);
    nmi->param = param;

    IF_FREE(label);
}


void menu_shutdown(void)
{
    Evas_List          *l;
    for (l = items; l; l = l->next)
    {
	    Menu_Item *mi = l->data;
	    IF_FREE(mi->label);
	    IF_FREE(mi->param);
	    IF_FREE(mi->pic);
	    free(mi);
    } 

    items = evas_list_free(items);
}

