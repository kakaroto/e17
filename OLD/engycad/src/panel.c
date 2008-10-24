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

typedef struct _Icon Icon;

struct _Icon
{
    char               *pic;
    char               *cmd;
    char               *hint;
    int                 x, y;
    Evas_Object        *o;
};

/* vars */
Eina_List          *icons = NULL;
Evas_Object        *oh, *oa, *on;
Evas_Object        *o_hbg, *o_hte;

/* protos */
void                load_icons(void);
void                panel_show(void);
void                panel_load_oh_and_oa(void);
Icon               *panel_item_load(int);

static void 
_panel_mouse_down(void *data, Evas *_e, Evas_Object *_o, void *event_info);

static void 
  _panel_mouse_up(void *data, Evas *_e, Evas_Object *_o, void *event_info);

static void 
  _panel_mouse_in(void *data, Evas *_e, Evas_Object *_o, void *event_info);

static void 
  _panel_mouse_out(void *data, Evas *_e, Evas_Object *_o, void *event_info);



void
panel_init(void)
{
    panel_load_oh_and_oa();
    load_icons();
    panel_show();
}

void
panel_configure(void)
{
    Eina_List          *l;
    Icon               *ic;

    for (l = icons; l; l = l->next)
      {
          ic = (Icon *) l->data;
          evas_object_move(ic->o,
                    (ic->x < 0) ? shell->w + ic->x : ic->x,
                    (ic->y < 0) ? shell->h + ic->y : ic->y);
      }
}

Icon               *
panel_item_load(int num)
{
    Evas               *e;
    Icon               *ic;
    char                buf[4096];
    char               *s;
    int                 i, n, res;

    e = shell->evas;

    ic = (Icon *) malloc(sizeof(Icon));
    ENGY_ASSERTS(ic, "malloc");
    memset(ic, 0, sizeof(Icon));

    sprintf(buf, "/icon/%d/pic", num);
    E_DB_STR_GET(shell->icons_file, buf, s, res);
    ENGY_ASSERT(res);
    if(s && (strlen(s)>4000)) s[4000]=0;
    if( s[0] != '/' ){
	char buf[4096];
	snprintf(buf, 4096,"%s/%s", shell->home,s); 
	FREE(s);
	s = DUP(buf);
    }    
    ic->pic = s;
    
    sprintf(buf, "/icon/%d/cmd", num);
    E_DB_STR_GET(shell->icons_file, buf, s, res);
    ENGY_ASSERT(res);
    if(s && (strlen(s)>4000)) s[4000]=0;
    ic->cmd = s;

    sprintf(buf, "/icon/%d/hint", num);
    E_DB_STR_GET(shell->icons_file, buf, s, res);
    ENGY_ASSERT(res);
    if(s && (strlen(s)>4000)) s[4000]=0;
    ic->hint = s;

    sprintf(buf, "/icon/%d/x", num);
    E_DB_INT_GET(shell->icons_file, buf, i, res);
    ENGY_ASSERT(res);
    ic->x = i;

    sprintf(buf, "/icon/%d/y", num);
    E_DB_INT_GET(shell->icons_file, buf, i, res);
    ENGY_ASSERT(res);
    ic->y = i;

    ic->o = evas_image_load(ic->pic);
    ENGY_ASSERT(ic->o);
    evas_object_move(ic->o,
              (ic->x < 0) ? shell->w + ic->x : ic->x,
              (ic->y < 0) ? shell->h + ic->y : ic->y);
    evas_object_color_set(ic->o, ALPHA2, ALPHA2, ALPHA2, ALPHA2);
    evas_object_layer_set(ic->o, 19);
    evas_object_stack_above(ic->o, oh);
//    evas_set_pass_events(e, ic->o,1);

    evas_object_event_callback_add(ic->o, EVAS_CALLBACK_MOUSE_DOWN, _panel_mouse_down, NULL);
    evas_object_event_callback_add(ic->o, EVAS_CALLBACK_MOUSE_UP, _panel_mouse_up, NULL);
    evas_object_event_callback_add(ic->o, EVAS_CALLBACK_MOUSE_IN, _panel_mouse_in, NULL);
    evas_object_event_callback_add(ic->o, EVAS_CALLBACK_MOUSE_OUT, _panel_mouse_out, NULL);

    return ic;
}

void
load_icons(void)
{
    int                 i, n, res;

    E_DB_INT_GET(shell->icons_file, "/icon/count", n, res);
    ENGY_ASSERTS(res, "icons loading");
    for (i = 0; i < n; i++)
      {
          Icon               *ic;

          ic = panel_item_load(i);
          icons = eina_list_append(icons, ic);
      }
}

void
panel_show(void)
{
    Eina_List          *l;
    Icon               *ic;

    for (l = icons; l; l = l->next)
      {
          ic = (Icon *) l->data;
          evas_object_show(ic->o);
      }
}

void
panel_load_oh_and_oa(void)
{
    Evas               *e;
    char               *s;
    int                 res;

    e = shell->evas;

    E_DB_STR_GET(shell->icons_file, "/hilighted", s, res);
    ENGY_ASSERT(res);
    if( s[0] != '/' ){
	char buf[4096];
	snprintf(buf, 4096,"%s/%s", shell->home,s);
	oh = evas_image_load(buf);
	FREE(s);
    } else {
	oh = evas_image_load(s);
	FREE(s);
    }
    ENGY_ASSERT(oh);
    evas_object_resize(oh, 24, 24);
    evas_object_image_fill_set(oh, 0, 0, 24, 24);
    evas_object_pass_events_set(oh, 1);
    evas_object_image_border_set(oh, 2, 2, 2, 2);
    evas_object_layer_set(oh, 19);

    E_DB_STR_GET(shell->icons_file, "/active", s, res);
    ENGY_ASSERT(res);
    if( s[0] != '/' ){
	char buf[4096];
	snprintf(buf, 4096,"%s/%s", shell->home,s); 
	oa = evas_image_load(buf);
	FREE(s);                        
    } else {                                            
	oa = evas_image_load(s);
	FREE(s);                                                
    }    
    ENGY_ASSERT(oa);
    evas_object_resize(oa, 24, 24);
    evas_object_image_fill_set(oa, 0, 0, 24, 24);
    evas_object_pass_events_set(oa, 1);
    evas_object_image_border_set(oa, 2, 2, 2, 2);
    evas_object_layer_set(oa, 19);

    evas_object_stack_above(oh, oa);

    o_hbg = evas_object_rectangle_add(e);
    evas_object_color_set(o_hbg, ALPHA3, ALPHA3, 200*ALPHA3/255, ALPHA3);
    evas_object_pass_events_set(o_hbg, 1);
    evas_object_layer_set(o_hbg, 20);

    o_hte = evas_object_text_add(e);
    evas_object_text_font_set(o_hte, "verdana", 9);
    evas_object_text_text_set(o_hte, "");
    evas_object_color_set(o_hte, 0, 0, 0, ALPHA4);
    evas_object_pass_events_set(o_hte, 1);
    evas_object_layer_set(o_hte, 20);
}

/* callbacks */
static void
_panel_mouse_down(void *data, Evas *_e, Evas_Object *_o, void *event_info)
{
    Evas               *e;
    Icon               *ic = NULL;
    Eina_List          *l;

    e = shell->evas;
    for (l = icons; l && (!ic); l = l->next)
      {
          Icon               *c;

          c = (Icon *) l->data;
          if (c->o == _o)
              ic = c;
      }

    evas_object_color_set(_o, ALPHA4, ALPHA4, ALPHA4, ALPHA4);
    evas_object_move(oa, (ic->x < 0) ? shell->w + ic->x : ic->x,
              (ic->y < 0) ? shell->h + ic->y : ic->y);
    evas_object_hide(oh);
    evas_object_show(oa);
}

static void
_panel_mouse_up(void *data, Evas *_e, Evas_Object *_o, void *event_info)
{
    Evas               *e;
    Icon               *ic = NULL;
    Eina_List          *l;

    e = shell->evas;
    for (l = icons; l && (!ic); l = l->next)
      {
          Icon               *c;

          c = (Icon *) l->data;
          if (c->o == _o)
              ic = c;
      }

    gui_put_string(DUP(ic->cmd));
    evas_object_color_set(_o, ALPHA3, ALPHA3, ALPHA3, ALPHA3);

    evas_object_show(oh);
    evas_object_hide(oa);
}

static void
_panel_mouse_in(void *data, Evas *_e, Evas_Object *_o, void *event_info)
{
    Evas               *e;
    Icon               *ic = NULL;
    Eina_List          *l;
    double              w;
    int			_x, _y;
    char *s;
    Evas_Event_Mouse_In *em = (Evas_Event_Mouse_In*)event_info;

    e = shell->evas;
    _x = em->canvas.x;
    _y = em->canvas.y;

    pointer_push_and_set(0x1400 + POINTER_ARROW);
    for (l = icons; l && (!ic); l = l->next)
      {
          Icon               *c;

          c = (Icon *) l->data;
          if (c->o == _o)
              ic = c;
      }

    evas_object_color_set(_o, ALPHA3, ALPHA3, ALPHA3, ALPHA3);
    evas_object_move(oh, (ic->x < 0) ? shell->w + ic->x : ic->x,
              (ic->y < 0) ? shell->h + ic->y : ic->y);
    evas_object_hide(oa);
    evas_object_show(oh);

    s = my_iconv(shell->dcd, _(ic->hint));
    evas_object_text_text_set(o_hte, s);
    FREE(s);
    w = evas_object_text_horiz_advance_get(o_hte);
    evas_object_move(o_hbg, _x + w * ((ic->x >= 0) ? 0 : -1),
              _y + 20 * ((ic->y >= 0) ? 1 : -1));
    evas_object_move(o_hte, _x + w * ((ic->x >= 0) ? 0 : -1) + 3,
              _y + 20 * ((ic->y >= 0) ? 1 : -1) - 6);
    evas_object_resize(o_hbg, w + 4, 12);
    evas_object_show(o_hbg);
    evas_object_show(o_hte);
}

static void
_panel_mouse_out(void *data, Evas *_e, Evas_Object *_o, void *event_info)
{
    Evas               *e;
    Icon               *ic = NULL;
    Eina_List          *l;

    e = shell->evas;
    pointer_pop(0x1400 + POINTER_ARROW);
    for (l = icons; l && (!ic); l = l->next)
      {
          Icon               *c;

          c = (Icon *) l->data;
          if (c->o == _o)
              ic = c;
      }

    evas_object_color_set(_o, ALPHA2, ALPHA2, ALPHA2, ALPHA2);
    evas_object_hide(oh);
    evas_object_hide(oa);

    evas_object_hide(o_hbg);
    evas_object_hide(o_hte);
}


void panel_shutdown(void)
{
    Eina_List *l;
    for (l = icons; l ; l = l->next)
      {
          Icon               *c = l->data;
	  IF_FREE(c->pic);
	  IF_FREE(c->cmd);
	  IF_FREE(c->hint);
	  FREE(c);
      }
    icons = eina_list_free(icons);
}
