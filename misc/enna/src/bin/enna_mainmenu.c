/*
 * enna_mainmenu.c
 * Copyright (C) Nicolas Aguirre 2006,2007,2008 <aguirre.nicolas@gmail.com>
 *
 * enna_mainmenu.c is free software copyrighted by Nicolas Aguirre.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name ``Nicolas Aguirre'' nor the name of any other
 *    contributor may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * enna_mainmenu.c IS PROVIDED BY Nicolas Aguirre ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL Nicolas Aguirre OR ANY OTHER CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <Evas.h>
#include <Ecore.h>
#include <Ecore_File.h>
#include <Ecore_Evas.h>
#include <Edje.h>
#include "enna_mainmenu.h"
#include "enna_config.h"
#include "enna_box.h"

#define SMART_NAME "enna_mainmenu"
#define API_ENTRY E_Smart_Data *sd; sd = evas_object_smart_data_get(obj); if ((!obj) || (!sd) || (evas_object_type_get(obj) && strcmp(evas_object_type_get(obj), SMART_NAME)))
#define INTERNAL_ENTRY E_Smart_Data *sd; sd = evas_object_smart_data_get(obj); if (!sd) return;
typedef struct _E_Smart_Data E_Smart_Data;
typedef struct _E_Smart_Item E_Smart_Item;

struct _E_Smart_Data
{
   Evas_Coord          x, y, w, h;
   Evas_Object        *edje;
   Evas_Object        *smart_obj;
   Evas_Object        *box_obj;
   Evas_List          *items;
   int                 selected;
   int                 active;
   Evas_Coord          icon_w, icon_h;
   unsigned char       selector:1;
   Ecore_Timer        *box_scroll;
   double              scroll_align;
   unsigned int        has_focus;
   int                 mouse_down;
};

struct _E_Smart_Item
{
   E_Smart_Data       *sd;
   Evas_Object        *edje;
   Evas_Object        *base_obj;
   Evas_Object        *icon_obj;
   Evas_Object        *miniplayer_obj;
   void                (*func) (void *data, void *data2);
   void                (*func_hilight) (void *data, void *data2);
   void               *data;
   void               *data2;
   unsigned char       miniplayer:1;
};

/* local subsystem functions */
static int          _scroll(void *data);
static void         _e_smart_event_mouse_down(void *data, Evas * e,
					      Evas_Object * obj,
					      void *event_info);
static void         _e_smart_event_mouse_up(void *data, Evas * e,
					    Evas_Object * obj,
					    void *event_info);
static void         _e_smart_event_mouse_in(void *data, Evas * e,
					    Evas_Object * obj,
					    void *event_info);
static void         _e_smart_event_mouse_out(void *data, Evas * e,
					     Evas_Object * obj,
					     void *event_info);
static void         _e_smart_reconfigure(E_Smart_Data * sd);
static void         _e_smart_add(Evas_Object * obj);
static void         _e_smart_del(Evas_Object * obj);
static void         _e_smart_move(Evas_Object * obj, Evas_Coord x,
				  Evas_Coord y);
static void         _e_smart_resize(Evas_Object * obj, Evas_Coord w,
				    Evas_Coord h);
static void         _e_smart_show(Evas_Object * obj);
static void         _e_smart_hide(Evas_Object * obj);
static void         _e_smart_color_set(Evas_Object * obj, int r, int g, int b,
				       int a);
static void         _e_smart_clip_set(Evas_Object * obj, Evas_Object * clip);
static void         _e_smart_clip_unset(Evas_Object * obj);
static void         _e_smart_init(void);

/* local subsystem globals */
static Evas_Smart  *_e_smart = NULL;

static int
_scroll(void *data)
{
   double              dif, d, f;
   Evas_Object        *obj;

   obj = (Evas_Object *) data;

   API_ENTRY           return 0;

   f = 0.1;
   enna_box_align_get(sd->box_obj, NULL, &d);

   dif = sd->scroll_align - d;
   if (dif < 0.0)
      dif = -dif;
   if (dif < 0.0001)
     {
	enna_box_align_set(sd->box_obj, 0.0, sd->scroll_align);

	sd->box_scroll = NULL;
	return 0;
     }
   else
      enna_box_align_set(sd->box_obj, 0.0,
			 (d * (1.0 - f)) + (sd->scroll_align * f));

   return 1;
}

/* externally accessible functions */
EAPI Evas_Object   *
enna_mainmenu_add(Evas * evas)
{
   _e_smart_init();
   return evas_object_smart_add(evas, _e_smart);
}

EAPI unsigned int
enna_mainmenu_has_focus_get(Evas_Object * obj)
{
   API_ENTRY           return 0;

   return sd->has_focus;
}

EAPI void
enna_mainmenu_has_focus_set(Evas_Object * obj, unsigned int focus)
{
   API_ENTRY           return;

   if (focus == sd->has_focus)
      return;

   sd->has_focus = focus;
   if (focus)
     {
	edje_object_signal_emit(sd->edje, "enna,state,focus", "enna");
	if (sd->active != sd->selected)
	  {
	     enna_mainmenu_selected_set(obj, sd->active);
	     edje_object_signal_emit(sd->edje, "enna,state,unfocus", "enna");
	     sd->has_focus = 0;
	  }
	return;
     }
   else
     {
	if (sd->active != sd->selected)
	  {
	     enna_mainmenu_selected_set(obj, sd->active);
	     edje_object_signal_emit(sd->edje, "enna,state,unfocus", "enna");
	     sd->has_focus = 0;
	     return;
	  }
	edje_object_signal_emit(sd->edje, "enna,state,unfocus", "enna");
	return;
     }
}

EAPI void
enna_mainmenu_icon_size_set(Evas_Object * obj, Evas_Coord w, Evas_Coord h)
{
   Evas_List          *l;

   API_ENTRY           return;

   if ((sd->icon_w == w) && (sd->icon_h == h))
      return;
   sd->icon_w = w;
   sd->icon_h = h;
   for (l = sd->items; l; l = l->next)
     {
	E_Smart_Item       *si;

	si = l->data;
	if (si->icon_obj)
	  {
	     Evas_Coord          mw = 0, mh = 0;

	     edje_extern_object_min_size_set(si->icon_obj, sd->icon_w,
					     sd->icon_h);
	     edje_object_part_swallow(si->base_obj, "enna.swallow.icon",
				      si->icon_obj);
	     edje_object_size_min_calc(si->base_obj, &mw, &mh);
	     enna_box_pack_options_set(si->icon_obj, 1, 1,	/* fill */
				       0, 1,	/* expand */
				       0.5, 0.5,	/* align */
				       mw, mh,	/* min */
				       99999, 99999	/* max */
		);
	  }
     }
}

EAPI void
enna_mainmenu_append(Evas_Object * obj, Evas_Object * icon,
		     const char *label, int miniplayer,
		     void (*func) (void *data, void *data2),
		     void (*func_hilight) (void *data, void *data2),
		     void *data, void *data2)
{
   E_Smart_Item       *si;
   Evas_Coord          mw = 0, mh = 0;

   API_ENTRY           return;

   si = (E_Smart_Item *) malloc(sizeof(E_Smart_Item));
   si->sd = sd;
   si->base_obj = edje_object_add(evas_object_evas_get(sd->smart_obj));

   if (miniplayer)
     {
	edje_object_file_set(si->base_obj, enna_config_theme_get(),
			     "widget/menu/item");
	si->icon_obj = NULL;
	si->miniplayer_obj = icon;
	if (si->miniplayer_obj)
	  {
	     edje_object_part_swallow(si->base_obj, "enna.swallow.miniplayer",
				      si->miniplayer_obj);
	     evas_object_show(si->miniplayer_obj);
	  }
     }
   else
     {
	edje_object_file_set(si->base_obj, enna_config_theme_get(),
			     "widget/menu/item");
	edje_object_part_text_set(si->base_obj, "menu.label", label);
	evas_object_show(si->base_obj);

	si->icon_obj = icon;
	si->miniplayer_obj = NULL;
	if (si->icon_obj)
	  {
	     edje_extern_object_min_size_set(si->icon_obj, sd->icon_w,
					     sd->icon_h);
	     edje_object_part_swallow(si->base_obj, "enna.swallow.icon",
				      si->icon_obj);
	     evas_object_show(si->icon_obj);
	  }
     }

   si->func = func;
   si->func_hilight = func_hilight;
   si->data = data;
   si->data2 = data2;
   si->miniplayer = miniplayer;
   sd->items = evas_list_append(sd->items, si);
   edje_object_size_min_calc(si->base_obj, &mw, &mh);
   edje_object_size_min_get(si->base_obj, &mw, &mh);
   enna_box_pack_end(sd->box_obj, si->base_obj);
   enna_box_pack_options_set(si->base_obj, 1, 1,	/* fill */
			     1, 1,	/* expand */
			     0.5, 0.5,	/* align */
			     mw, mh,	/* min */
			     99999, 9999	/* max */
      );

   //enna_box_align_set(sd->box_obj, 0, 0.5);
   evas_object_lower(si->base_obj);

   evas_object_event_callback_add(si->base_obj, EVAS_CALLBACK_MOUSE_DOWN,
				  _e_smart_event_mouse_down, si);
   evas_object_event_callback_add(si->base_obj, EVAS_CALLBACK_MOUSE_UP,
				  _e_smart_event_mouse_up, si);
   evas_object_event_callback_add(si->base_obj, EVAS_CALLBACK_MOUSE_IN,
				  _e_smart_event_mouse_in, si);
   evas_object_event_callback_add(si->base_obj, EVAS_CALLBACK_MOUSE_OUT,
				  _e_smart_event_mouse_out, si);
   evas_object_show(si->base_obj);
}

EAPI void
enna_mainmenu_prepend(Evas_Object * obj, Evas_Object * icon,
		      const char *label, int miniplayer,
		      void (*func) (void *data, void *data2),
		      void (*func_hilight) (void *data, void *data2),
		      void *data, void *data2)
{
   E_Smart_Item       *si;
   Evas_Coord          mw = 0, mh = 0;

   API_ENTRY           return;

   si = (E_Smart_Item *) malloc(sizeof(E_Smart_Item));
   si->sd = sd;
   si->base_obj = edje_object_add(evas_object_evas_get(sd->smart_obj));

   if (miniplayer)
     {
	edje_object_file_set(si->base_obj, enna_config_theme_get(),
			     "widget/menu/miniplayer");
	si->icon_obj = NULL;
	si->miniplayer_obj = icon;
	if (si->miniplayer_obj)
	  {
	     edje_object_part_swallow(si->base_obj, "enna.swallow.miniplayer",
				      si->miniplayer_obj);
	     evas_object_show(si->miniplayer_obj);
	  }
     }
   else
     {
	edje_object_file_set(si->base_obj, enna_config_theme_get(),
			     "widget/menu/item");
	edje_object_part_text_set(si->base_obj, "menu.label", label);
	si->icon_obj = icon;
	if (si->icon_obj)
	  {
	     edje_extern_object_min_size_set(si->icon_obj, sd->icon_w,
					     sd->icon_h);
	     edje_object_part_swallow(si->base_obj, "enna.swallow.icon",
				      si->icon_obj);
	     evas_object_show(si->icon_obj);
	  }
     }

   si->func = func;
   si->func_hilight = func_hilight;
   si->data = data;
   si->data2 = data2;
   si->miniplayer = miniplayer;
   sd->items = evas_list_prepend(sd->items, si);
   edje_object_size_min_calc(si->base_obj, &mw, &mh);
   edje_object_size_min_get(si->base_obj, &mw, &mh);
   enna_box_pack_start(sd->box_obj, si->base_obj);
   enna_box_pack_options_set(si->base_obj, 1, 1,	/* fill */
			     1, 1,	/* expand */
			     0.5, 0.5,	/* align */
			     mw, mh,	/* min */
			     99999, 9999	/* max */
      );

   //   enna_box_align_set(sd->box_obj, 0, 0.5);
   //evas_object_lower(si->base_obj);
   evas_object_event_callback_add(si->base_obj, EVAS_CALLBACK_MOUSE_DOWN,
				  _e_smart_event_mouse_down, si);
   evas_object_event_callback_add(si->base_obj, EVAS_CALLBACK_MOUSE_UP,
				  _e_smart_event_mouse_up, si);
   evas_object_show(si->base_obj);
}

EAPI void
enna_mainmenu_center(Evas_Object * obj)
{
   Evas_Coord          ow = 0, oh = 0, bw = 0, bh = 0;
   API_ENTRY           return;

   evas_object_geometry_get(sd->smart_obj, NULL, NULL, &ow, &oh);
   evas_object_geometry_get(sd->box_obj, NULL, NULL, &bw, &bh);
}

EAPI void
enna_mainmenu_active_set(Evas_Object * obj, int n)
{
   E_Smart_Item       *si;
   API_ENTRY           return;

   enna_mainmenu_selected_set(obj, n);

   si = evas_list_nth(sd->items, sd->selected);
   if (si)
     {
	if (si->func)
	  {
	     sd->active = sd->selected;
	     enna_mainmenu_has_focus_set(sd->smart_obj, 0);
	     si->func(si->data, si->data2);
	  }
     }
}

EAPI void
enna_mainmenu_selected_set(Evas_Object * obj, int n)
{
   E_Smart_Item       *si;

   API_ENTRY           return;

   if (!sd->items)
      return;
   if (n >= evas_list_count(sd->items))
      n = evas_list_count(sd->items) - 1;
   else if (n < 0)
      n = 0;
   if (sd->selected == n)
      return;

   si = evas_list_nth(sd->items, sd->selected);
   if (si)
      edje_object_signal_emit(si->base_obj, "unselected", "enna");
   sd->selected = n;
   si = evas_list_nth(sd->items, sd->selected);
   if (si)
     {
	Evas_Coord          bh = 0;

	enna_box_min_size_get(sd->box_obj, NULL, &bh);
	if (sd->h && sd->h < bh)
	  {
	     if (!sd->box_scroll)
		sd->box_scroll = ecore_timer_add(1.0 / 30.0, _scroll, obj);
	     sd->scroll_align =
		1.0 -
		((double)sd->selected /
		 ((double)evas_list_count(sd->items) - 1.0));
	  }

	//evas_object_raise (si->base_obj);

	if (si->miniplayer_obj)
	  {
	     edje_object_signal_emit(si->base_obj, "selected", "enna");
	     edje_object_signal_emit(si->miniplayer_obj, "selected", "enna");
	  }
	else
	   edje_object_signal_emit(si->base_obj, "selected", "enna");
	if (si->func_hilight)
	   si->func_hilight(si->data, si->data2);
	if (sd->selector)
	  {
	     if (si->func)
		si->func(si->data, si->data2);
	  }
     }
}

EAPI int
enna_mainmenu_selected_get(Evas_Object * obj)
{
   API_ENTRY           return -1;

   if (!sd->items)
      return -1;
   return sd->selected;
}

EAPI void
enna_mainmenu_unselect(Evas_Object * obj)
{
   E_Smart_Item       *si;

   API_ENTRY           return;

   if (!sd->items)
      return;
   si = evas_list_nth(sd->items, sd->selected);
   if (si)
      edje_object_signal_emit(si->base_obj, "unselected", "enna");
   sd->selected = -1;
}

EAPI const char    *
enna_mainmenu_selected_label_get(Evas_Object * obj)
{
   E_Smart_Item       *si;

   API_ENTRY           return NULL;

   if (!sd->items)
      return NULL;
   si = evas_list_nth(sd->items, sd->selected);
   if (si)
      return edje_object_part_text_get(si->base_obj, "menu.label");
   return NULL;
}

EAPI Evas_Object   *
enna_mainmenu_selected_icon_get(Evas_Object * obj)
{
   E_Smart_Item       *si;

   API_ENTRY           return NULL;

   if (!sd->items)
      return NULL;
   si = evas_list_nth(sd->items, sd->selected);
   if (si && si->icon_obj)
      return si->icon_obj;
   return NULL;
}

EAPI void          *
enna_mainmenu_selected_data_get(Evas_Object * obj)
{
   E_Smart_Item       *si;

   API_ENTRY           return NULL;

   if (!sd->items)
      return NULL;
   si = evas_list_nth(sd->items, sd->selected);
   if (si)
      return si->data;
   return NULL;
}

EAPI void          *
enna_mainmenu_selected_data2_get(Evas_Object * obj)
{
   E_Smart_Item       *si;

   API_ENTRY           return NULL;

   if (!sd->items)
      return NULL;
   si = evas_list_nth(sd->items, sd->selected);
   if (si)
      return si->data2;
   return NULL;
}

EAPI void
enna_mainmenu_selected_geometry_get(Evas_Object * obj, Evas_Coord * x,
				    Evas_Coord * y, Evas_Coord * w,
				    Evas_Coord * h)
{
   E_Smart_Item       *si;

   API_ENTRY           return;

   si = evas_list_nth(sd->items, sd->selected);
   if (si)
     {
	evas_object_geometry_get(si->base_obj, x, y, w, h);
	*x -= sd->x;
	*y -= sd->y;
     }
}

EAPI void
enna_mainmenu_min_size_get(Evas_Object * obj, Evas_Coord * w, Evas_Coord * h)
{
   API_ENTRY           return;

   enna_box_min_size_get(sd->box_obj, w, h);
}

EAPI void
enna_mainmenu_selector_set(Evas_Object * obj, int selector)
{
   API_ENTRY           return;

   sd->selector = selector;
}

EAPI int
enna_mainmenu_selector_get(Evas_Object * obj)
{
   API_ENTRY           return 0;

   return sd->selector;
}

EAPI void
enna_mainmenu_remove_num(Evas_Object * obj, int n)
{
   E_Smart_Item       *si;

   API_ENTRY           return;

   if (!sd->items)
      return;
   si = evas_list_nth(sd->items, n);
   if (si)
     {
	sd->items = evas_list_remove(sd->items, si);
	/*      if (enna_mainmenu_selected_get(obj) == n)
	 * sd->selected = -1; */
	/*if (si->icon_obj)
	 * evas_object_del(si->icon_obj); */
	evas_object_del(si->base_obj);
	evas_object_del(si->icon_obj);
	//evas_object_del(si->miniplayer_obj);
	//free(si->func);
	//free(si->func_hilight);
	free(si);
     }
}

EAPI void
enna_mainmenu_remove_label(Evas_Object * obj, const char *label)
{
   E_Smart_Item       *si;
   Evas_List          *l;
   int                 i;

   API_ENTRY           return;

   if (!sd->items)
      return;
   if (!label)
      return;
   for (i = 0, l = sd->items; l; l = l->next, i++)
     {
	si = l->data;
	if (si)
	  {
	     char               *t;

	     t = strdup(edje_object_part_text_get(si->base_obj, "menu.label"));
	     if (!strcmp(t, label))
	       {
		  if (si->icon_obj)
		     evas_object_del(si->icon_obj);
		  evas_object_del(si->base_obj);
		  sd->items = evas_list_remove(sd->items, si);
		  free(si);
		  break;
	       }
	     free(t);
	  }
     }
}

EAPI const char    *
enna_mainmenu_nth_label_get(Evas_Object * obj, int n)
{
   E_Smart_Item       *si;

   API_ENTRY           return NULL;

   if (!sd->items)
      return NULL;
   si = evas_list_nth(sd->items, n);
   if (si)
      return edje_object_part_text_get(si->base_obj, "menu.label");
   return NULL;
}

EAPI void
enna_mainmenu_nth_label_set(Evas_Object * obj, int n, const char *label)
{
   E_Smart_Item       *si;

   API_ENTRY           return;

   if (!sd->items)
      return;
   si = evas_list_nth(sd->items, n);
   if (si)
      edje_object_part_text_set(si->base_obj, "menu.label", label);
}

EAPI Evas_Object   *
enna_mainmenu_nth_icon_get(Evas_Object * obj, int n)
{
   E_Smart_Item       *si;

   API_ENTRY           return NULL;

   if (!sd->items)
      return NULL;
   si = evas_list_nth(sd->items, n);
   if (si && si->icon_obj)
      return si->icon_obj;
   return NULL;
}

EAPI void
enna_mainmenu_nth_icon_set(Evas_Object * obj, int n, Evas_Object * icon)
{
   E_Smart_Item       *si;

   API_ENTRY           return;

   if (!sd->items)
      return;
   si = evas_list_nth(sd->items, n);
   if (si && !si->miniplayer_obj)
     {
	if (si->icon_obj)
	  {
	     edje_object_part_unswallow(si->base_obj, si->icon_obj);
	     evas_object_hide(si->icon_obj);
	     evas_object_del(si->icon_obj);
	  }

	si->icon_obj = icon;
	if (si->icon_obj)
	  {
	     edje_extern_object_min_size_set(si->icon_obj, sd->icon_w,
					     sd->icon_h);
	     edje_object_part_swallow(si->base_obj, "enna.swallow.icon",
				      si->icon_obj);
	     evas_object_show(si->icon_obj);
	  }
     }
}

EAPI int
enna_mainmenu_count(Evas_Object * obj)
{
   API_ENTRY           return 0;

   return evas_list_count(sd->items);
}

EAPI void
enna_mainmenu_clear(Evas_Object * obj)
{
   INTERNAL_ENTRY;
   while (sd->items)
     {
	E_Smart_Item       *si;

	si = sd->items->data;
	sd->items = evas_list_remove_list(sd->items, sd->items);
	if (si->icon_obj)
	   evas_object_del(si->icon_obj);
	if (si->miniplayer_obj)
	   evas_object_del(si->miniplayer_obj);
	evas_object_del(si->base_obj);
	free(si);
     }
   sd->selected = -1;
}

EAPI int
enna_mainmenu_nth_is_miniplayer(Evas_Object * obj, int n)
{
   E_Smart_Item       *si;

   API_ENTRY           return 0;

   if (!sd->items)
      return 0;
   si = evas_list_nth(sd->items, n);
   if (si)
      return si->miniplayer;
   return 0;
}

EAPI void
enna_mainmenu_process_event(Evas_Object * obj, enna_event event)
{
   API_ENTRY           return;

   switch (event)
     {
     case enna_event_left:
	{
	   int                 n, ns;
	   E_Smart_Item       *si;

	   ns = enna_mainmenu_selected_get(sd->smart_obj);
	   n = ns;
	   do
	     {
		if (n == 0)
		  {
		     n = ns;
		     break;
		  }
		--n;
		si = evas_list_nth(sd->items, n);
	     }
	   while (si && 0);
	   if (n != ns)
	      enna_mainmenu_selected_set(sd->smart_obj, n);

	   if (sd->active == sd->selected)
	      sd->has_focus = 0;
	   else
	      sd->has_focus = 1;

	   break;
	}
     case enna_event_right:
	{
	   int                 n, ns;
	   E_Smart_Item       *si;

	   ns = enna_mainmenu_selected_get(sd->smart_obj);
	   n = ns;
	   do
	     {
		if (n == (evas_list_count(sd->items) - 1))
		  {
		     n = ns;
		     break;
		  }
		++n;
		si = evas_list_nth(sd->items, n);
	     }
	   while (si && 0);
	   if (n != ns)
	      enna_mainmenu_selected_set(sd->smart_obj, n);
	   if (sd->active == sd->selected)
	      sd->has_focus = 0;
	   else
	      sd->has_focus = 1;
	   break;
	}
     case enna_event_enter:
	{
	   E_Smart_Item       *si;

	   si = evas_list_nth(sd->items, sd->selected);
	   if (si)
	     {
		if (si->func)
		  {
		     sd->active = sd->selected;
		     enna_mainmenu_has_focus_set(sd->smart_obj, 0);
		     si->func(si->data, si->data2);
		  }

	     }
	   break;
	}
     case enna_event_end:
	{
	   int                 n, ns;
	   E_Smart_Item       *si;

	   ns = enna_mainmenu_selected_get(sd->smart_obj);
	   n = evas_list_count(sd->items);
	   do
	     {
		if (n == 0)
		  {
		     n = ns;
		     break;
		  }
		--n;
		si = evas_list_nth(sd->items, n);
	     }
	   while (si && 0);
	   if (n != ns)
	      enna_mainmenu_selected_set(sd->smart_obj, n);
	   break;
	}
     case enna_event_home:
	{
	   int                 n, ns;
	   E_Smart_Item       *si;

	   ns = enna_mainmenu_selected_get(sd->smart_obj);
	   n = -1;
	   do
	     {
		if (n == (evas_list_count(sd->items) - 1))
		  {
		     n = ns;
		     break;
		  }
		++n;
		si = evas_list_nth(sd->items, n);
	     }
	   while (si && 0);
	   if (n != ns)
	      enna_mainmenu_selected_set(sd->smart_obj, n);
	   break;
	}
     default:
	{
	   break;
	}
     }
}

/* local subsystem functions */
#if 0
static void
_e_smart_event_mouse_down(void *data, Evas * e, Evas_Object * obj,
			  void *event_info)
{
   Evas_Event_Mouse_Down *ev;
   E_Smart_Item       *si;
   Evas_List          *l;
   int                 i;
   int                 n;

   si = data;
   ev = event_info;

   for (i = 0, l = si->sd->items; l; l = l->next, i++)
     {
	if (l->data == si)
	  {
	     enna_mainmenu_selected_set(si->sd->smart_obj, i);
	  }
     }
}

static void
_e_smart_event_mouse_up(void *data, Evas * e, Evas_Object * obj,
			void *event_info)
{
   Evas_Event_Mouse_Up *ev;
   E_Smart_Item       *si;
   Evas_List          *l;
   int                 i;

   si = data;
   ev = event_info;

   for (i = 0, l = si->sd->items; l; l = l->next, i++)
     {
	if (l->data == si)
	  {
	     if (si->func)
	       {
		  si->sd->active = si->sd->selected;
		  enna_mainmenu_has_focus_set(si->sd->smart_obj, 0);
		  si->func(si->data, si->data2);
	       }
	  }
     }

}
#endif

static void
_e_smart_event_mouse_down(void *data, Evas * e, Evas_Object * obj,
			  void *event_info)
{
   Evas_Event_Mouse_Down *ev;
   E_Smart_Item       *si;
   Evas_List          *l;
   int                 i;

   si = data;
   ev = event_info;
   si->sd->mouse_down = 1;
   if ((ev->button == 1) && (ev->flags == EVAS_BUTTON_NONE))
     {
	for (i = 0, l = si->sd->items; l; l = l->next, i++)
	  {
	     if (l->data == si)
	       {
		  enna_mainmenu_selected_set(si->sd->smart_obj, i);
	       }
	  }
     }
}

static void
_e_smart_event_mouse_in(void *data, Evas * e, Evas_Object * obj,
			void *event_info)
{
   Evas_Event_Mouse_In *ev;
   E_Smart_Item       *si;
   Evas_List          *l;
   int                 i;

   si = data;
   ev = event_info;

   if (!si->sd->mouse_down)
      return;
   for (i = 0, l = si->sd->items; l; l = l->next, i++)
     {
	if (l->data == si)
	   enna_mainmenu_selected_set(si->sd->smart_obj, i);
     }
}

static void
_e_smart_event_mouse_out(void *data, Evas * e, Evas_Object * obj,
			 void *event_info)
{
   Evas_Event_Mouse_Out *ev;
   E_Smart_Item       *si;

   si = data;
   ev = event_info;

}

static void
_e_smart_event_mouse_up(void *data, Evas * e, Evas_Object * obj,
			void *event_info)
{
   Evas_Event_Mouse_Up *ev;
   E_Smart_Item       *si;
   int                 i;
   Evas_List          *l;

   si = data;
   ev = event_info;
   if (!si->sd->mouse_down)
      return;
   else
      si->sd->mouse_down = 0;
   if ((ev->button == 1) && (ev->flags == EVAS_BUTTON_NONE))
     {
	for (i = 0, l = si->sd->items; l; l = l->next, i++)
	  {
	     if (l->data == si)
	       {
		  if (si->func)
		    {
		       si->func(si->data, si->data2);
		    }
	       }
	  }
     }
   else if ((ev->button == 3) && (ev->flags == EVAS_BUTTON_NONE))
     {

     }
}

static void
_e_smart_reconfigure(E_Smart_Data * sd)
{
   evas_object_move(sd->edje, sd->x, sd->y);
   evas_object_resize(sd->edje, sd->w, sd->h);
}

static void
_e_smart_add(Evas_Object * obj)
{
   E_Smart_Data       *sd;
   Evas_Coord          x, y, w, h;

   sd = calloc(1, sizeof(E_Smart_Data));
   if (!sd)
      return;
   evas_object_smart_data_set(obj, sd);

   sd->smart_obj = obj;
   sd->x = 0;
   sd->y = 0;
   sd->w = 0;
   sd->h = 0;

   sd->icon_w = 24;
   sd->icon_h = 24;

   sd->selected = -1;
   sd->active = 0;
   sd->box_scroll = NULL;
   sd->scroll_align = 0.0;
   sd->has_focus = 0;
   sd->edje = edje_object_add(evas_object_evas_get(obj));
   sd->mouse_down = 0;
   edje_object_file_set(sd->edje, enna_config_theme_get(), "mainmenu");
   sd->box_obj = enna_box_add(evas_object_evas_get(obj));
   enna_box_align_set(sd->box_obj, 0.0, 0.0);
   enna_box_orientation_set(sd->box_obj, 1);
   enna_box_homogenous_set(sd->box_obj, 0);
   evas_object_smart_member_add(sd->edje, obj);

   edje_object_part_swallow(sd->edje, "menu.swallow", sd->box_obj);
   evas_object_show(sd->box_obj);
   evas_object_show(sd->edje);
   evas_object_geometry_get(sd->box_obj, &x, &y, &w, &h);

}

static void
_e_smart_del(Evas_Object * obj)
{
   INTERNAL_ENTRY;
   while (sd->items)
     {
	E_Smart_Item       *si;

	si = sd->items->data;
	sd->items = evas_list_remove_list(sd->items, sd->items);
	if (si->icon_obj)
	   evas_object_del(si->icon_obj);
	evas_object_del(si->base_obj);
	free(si);
     }
   evas_object_del(sd->edje);
   free(sd);
}

static void
_e_smart_move(Evas_Object * obj, Evas_Coord x, Evas_Coord y)
{
   INTERNAL_ENTRY;
   if ((sd->x == x) && (sd->y == y))
      return;
   sd->x = x;
   sd->y = y;
   _e_smart_reconfigure(sd);
}

static void
_e_smart_resize(Evas_Object * obj, Evas_Coord w, Evas_Coord h)
{
   INTERNAL_ENTRY;

   if ((sd->w == w) && (sd->h == h))
      return;
   sd->w = w;
   sd->h = h;

   if (evas_list_count(sd->items) * 75 > sd->h)
      enna_box_align_set(sd->box_obj, 1, 0.0);
   else
      enna_box_align_set(sd->box_obj, 0.5, 0);

   _e_smart_reconfigure(sd);

}

static void
_e_smart_show(Evas_Object * obj)
{
   INTERNAL_ENTRY;
   evas_object_show(sd->edje);
}

static void
_e_smart_hide(Evas_Object * obj)
{
   INTERNAL_ENTRY;
   evas_object_hide(sd->edje);
}

static void
_e_smart_color_set(Evas_Object * obj, int r, int g, int b, int a)
{
   INTERNAL_ENTRY;
   evas_object_color_set(sd->edje, r, g, b, a);
}

static void
_e_smart_clip_set(Evas_Object * obj, Evas_Object * clip)
{
   INTERNAL_ENTRY;
   evas_object_clip_set(sd->edje, clip);
}

static void
_e_smart_clip_unset(Evas_Object * obj)
{
   INTERNAL_ENTRY;
   evas_object_clip_unset(sd->edje);
}

/* never need to touch this */

static void
_e_smart_init(void)
{
   if (_e_smart)
      return;
   static const Evas_Smart_Class sc = {
      SMART_NAME,
      EVAS_SMART_CLASS_VERSION,
      _e_smart_add,
      _e_smart_del,
      _e_smart_move,
      _e_smart_resize,
      _e_smart_show,
      _e_smart_hide,
      _e_smart_color_set,
      _e_smart_clip_set,
      _e_smart_clip_unset,
      NULL
   };
   _e_smart = evas_smart_class_new(&sc);
}
