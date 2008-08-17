/*
 * enna_list.c
 * Copyright (C) Nicolas Aguirre 2006,2007,2008 <aguirre.nicolas@gmail.com>
 *
 * enna_list.c is free software copyrighted by Nicolas Aguirre.
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
 * enna_list.c IS PROVIDED BY Nicolas Aguirre ``AS IS'' AND ANY EXPRESS
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

/* derived from e_ilist.c */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <Evas.h>
#include <Ecore.h>
#include <Ecore_File.h>
#include <Ecore_Evas.h>
#include <Ecore_Str.h>
#include <Edje.h>
#include "enna.h"
#include "enna_config.h"
#include "enna_list.h"
#include "enna_box.h"
#include "enna_mediaplayer.h"
#include "enna_util.h"
#include "enna_lirc.h"
#include <math.h>

#define TIME 1.5

#define SMART_NAME "enna_list"
#define API_ENTRY E_Smart_Data *sd; sd = evas_object_smart_data_get(obj); if ((!obj) || (!sd) || (evas_object_type_get(obj) && strcmp(evas_object_type_get(obj), SMART_NAME)))
#define INTERNAL_ENTRY E_Smart_Data *sd; sd = evas_object_smart_data_get(obj); if (!sd) return;
typedef struct _E_Smart_Data E_Smart_Data;
typedef struct _E_Smart_Item E_Smart_Item;

struct _E_Smart_Data
{
   Evas_Coord          x, y, w, h;

   Evas_Object        *smart_obj;
   Evas_Object        *edje;
   Evas_Object        *box_obj;
   Evas_List          *items;
   Evas_Object        *letter;
   int                 selected;
   Evas_Coord          icon_w, icon_h;
   unsigned char       selector:1;
   Ecore_Timer        *box_scroll;
   Ecore_Timer        *select_up_timer;
   Ecore_Timer        *select_down_timer;
   double              scroll_align;
   int                 nb_items_shown;
   int                 mouse_down;
   unsigned int        letter_mode;
   Ecore_Timer        *letter_timer;
   unsigned int        letter_event_nbr;
   enna_event          letter_event;

};

struct _E_Smart_Item
{
   E_Smart_Data       *sd;
   Evas_Object        *base_obj;
   Evas_Object        *icon_obj;
   void                (*func) (void *data, void *data2);
   void                (*func_hilight) (void *data, void *data2);
   void               *data;
   void               *data2;
   unsigned char       in_playlist:1;
   unsigned char       header:1;
};

/* local subsystem functions */
static int          _scroll(void *data);
static int          _letter_timer_cb(void *data);
static int          _is_in_pl(E_Smart_Data * sd, char *filename);
static void         enna_list_selected_set_now(Evas_Object * obj, int n);
static void         _e_smart_event_mouse_wheel(void *data, Evas * e,
					       Evas_Object * obj,
					       void *event_info);
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
_letter_timer_cb(void *data)
{
   E_Smart_Data       *sd;
   int                 i;
   Evas_List          *l;

   sd = data;

   edje_object_signal_emit(sd->edje, "enna,letter,hide", "enna");
   sd->letter_mode = 0;
   for (i = 0, l = sd->items; l; l = l->next, i++)
     {
	char               *label;
	char               *letter;
	E_Smart_Item       *si;

	si = l->data;
	label =
	   (char *)edje_object_part_text_get(si->base_obj, "enna.text.label");
	letter =
	   (char *)edje_object_part_text_get(sd->edje, "enna.text.letter");
	if ((letter && label)
	    && (letter[0] == label[0] || letter[0] == label[0] + 32
		|| letter[0] + 32 == label[0]))
	  {
	     enna_list_selected_set(sd->smart_obj, i);
	     break;
	  }
     }
   return 0;
}

static int
_scroll(void *data)
{
   double              dif, d, f;
   Evas_Object        *obj;

   obj = (Evas_Object *) data;

   API_ENTRY           return 0;

   f = 0.08;
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
enna_list_add(Evas * evas)
{
   _e_smart_init();
   return evas_object_smart_add(evas, _e_smart);
}

EAPI void
enna_list_icon_size_set(Evas_Object * obj, Evas_Coord w, Evas_Coord h)
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
	     edje_object_part_swallow(si->base_obj, "icon.swallow",
				      si->icon_obj);
	     edje_object_size_min_calc(si->base_obj, &mw, &mh);
	     enna_box_pack_options_set(si->icon_obj, 1, 1,	/* fill */
				       1, 0,	/* expand */
				       0.5, 0.5,	/* align */
				       mw, mh,	/* min */
				       99999, 99999	/* max */
		);
	  }
     }
}

EAPI void
enna_list_append_with_icon_name(Evas_Object * obj, const char *icon_name,
				const char *label, void (*func) (void *data,
								 void *data2),
				void (*func_hilight) (void *data, void *data2),
				void *data, void *data2)
{

   API_ENTRY           return;

   Evas_Object        *icon = NULL;

   icon = edje_object_add(evas_object_evas_get(sd->edje));
   edje_object_file_set(icon, enna_config_theme_get(), icon_name);
   enna_list_append(obj, icon, label, 0, func, func_hilight, data, data2);

}

EAPI void
enna_list_append(Evas_Object * obj, Evas_Object * icon, const char *label,
		 int header, void (*func) (void *data, void *data2),
		 void (*func_hilight) (void *data, void *data2), void *data,
		 void *data2)
{
   E_Smart_Item       *si;
   Evas_Coord          mw = 0, mh = 0;
   API_ENTRY           return;

   si = (E_Smart_Item *) malloc(sizeof(E_Smart_Item));
   si->sd = sd;
   si->base_obj = edje_object_add(evas_object_evas_get(sd->smart_obj));

   if (header)
      edje_object_file_set(si->base_obj, enna_config_theme_get(),
			   "enna/list_header");
   else if (evas_list_count(sd->items) & 0x1)
      edje_object_file_set(si->base_obj, enna_config_theme_get(),
			   "widget/list/item");
   else
      edje_object_file_set(si->base_obj, enna_config_theme_get(),
			   "widget/list/item");
   edje_object_part_text_set(si->base_obj, "enna.text.label", label);

   si->icon_obj = icon;
   if (si->icon_obj)
     {
	edje_extern_object_min_size_set(si->icon_obj, sd->icon_w, sd->icon_h);
	edje_object_part_swallow(si->base_obj, "icon.swallow", si->icon_obj);
	evas_object_show(si->icon_obj);
     }
   si->func = func;
   si->func_hilight = func_hilight;
   si->data = data;
   si->data2 = data2;
   si->header = header;
   si->in_playlist = _is_in_pl(sd, data2);
   if (si->in_playlist)
      edje_object_signal_emit(si->base_obj, "enna,state,inpl", "enna");
   else
      edje_object_signal_emit(si->base_obj, "enna,state,notinpl", "enna");

   sd->items = evas_list_append(sd->items, si);

   edje_object_size_min_calc(si->base_obj, &mw, &mh);
   edje_object_size_min_get(si->base_obj, &mw, &mh);

   enna_box_pack_end(sd->box_obj, si->base_obj);
   enna_box_pack_options_set(si->base_obj, 1, 0,	/* fill */
			     1, 0,	/* expand */
			     0.5, 0.5,	/* align */
			     300, 50,	/* min */
			     99999, 9999	/* max */
      );

   evas_object_lower(si->base_obj);

   evas_object_event_callback_add(si->base_obj, EVAS_CALLBACK_MOUSE_WHEEL,
				  _e_smart_event_mouse_wheel, si);
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
enna_list_center(Evas_Object * obj)
{
   Evas_Coord          ow = 0, oh = 0, bw = 0, bh = 0;
   API_ENTRY           return;

   evas_object_geometry_get(sd->smart_obj, NULL, NULL, &ow, &oh);
   evas_object_geometry_get(sd->box_obj, NULL, NULL, &bw, &bh);

}

EAPI void
enna_list_selected_set(Evas_Object * obj, int n)
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
//        dbg("box size ! %d height : %d\n", bh, sd->h);
	if (sd->h && sd->h < bh)
	  {
	     if (!sd->box_scroll)
		sd->box_scroll = ecore_timer_add(1.0 / 30.0, _scroll, obj);
	     sd->scroll_align =
		1 -
		((double)sd->selected) / ((double)evas_list_count(sd->items) -
					  1);
	  }

	evas_object_raise(si->base_obj);
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

EAPI void
enna_list_selected_set_now(Evas_Object * obj, int n)
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
	evas_object_raise(si->base_obj);
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
enna_list_selected_get(Evas_Object * obj)
{
   API_ENTRY           return -1;

   if (!sd->items)
      return -1;
   return sd->selected;
}

EAPI void
enna_list_in_playlist_set(Evas_Object * obj, unsigned char in_pl, int n)
{
   E_Smart_Item       *si;
   API_ENTRY           return;

   if (!sd->items)
      return;
   if (n >= evas_list_count(sd->items))
      n = evas_list_count(sd->items) - 1;
   else if (n < 0)
      n = 0;
   si = evas_list_nth(sd->items, sd->selected);
   if (si)
     {
	if (si->in_playlist == in_pl)
	   return;
	else if (in_pl)
	  {
	     edje_object_signal_emit(si->base_obj, "enna,state,inpl", "enna");
	  }
	else
	   edje_object_signal_emit(si->base_obj, "enna,state,notinpl", "enna");

	si->in_playlist = in_pl;
     }
}

EAPI void
enna_list_unselect(Evas_Object * obj)
{
   E_Smart_Item       *si;

   API_ENTRY           return;

   if (!sd->items)
      return;
   si = evas_list_nth(sd->items, sd->selected);
   if (si)
      edje_object_signal_emit(si->base_obj, "unselected", "e");
   sd->selected = -1;
}

EAPI const char    *
enna_list_selected_label_get(Evas_Object * obj)
{
   E_Smart_Item       *si;

   API_ENTRY           return NULL;

   if (!sd->items)
      return NULL;
   si = evas_list_nth(sd->items, sd->selected);
   if (si)
      return edje_object_part_text_get(si->base_obj, "enna.text.label");
   return NULL;
}

EAPI const char    *
enna_list_label_get(Evas_Object * obj, int n)
{
   E_Smart_Item       *si;

   API_ENTRY           return NULL;

   if (!sd->items)
      return NULL;
   si = evas_list_nth(sd->items, n);
   if (si)
      return edje_object_part_text_get(si->base_obj, "enna.text.label");
   return NULL;
}

EAPI Evas_Object   *
enna_list_selected_icon_get(Evas_Object * obj)
{
   E_Smart_Item       *si;

   API_ENTRY           return NULL;

   if (!sd->items)
      return NULL;
   si = evas_list_nth(sd->items, sd->selected);
   if (si)
      return si->icon_obj;
   return NULL;
}

EAPI void          *
enna_list_selected_data_get(Evas_Object * obj)
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
enna_list_selected_data2_get(Evas_Object * obj)
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
enna_list_selected_geometry_get(Evas_Object * obj, Evas_Coord * x,
				Evas_Coord * y, Evas_Coord * w, Evas_Coord * h)
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
enna_list_min_size_get(Evas_Object * obj, Evas_Coord * w, Evas_Coord * h)
{
   API_ENTRY           return;

   enna_box_min_size_get(sd->box_obj, w, h);
}

EAPI void
enna_list_selector_set(Evas_Object * obj, int selector)
{
   API_ENTRY           return;

   sd->selector = selector;
}

EAPI int
enna_list_selector_get(Evas_Object * obj)
{
   API_ENTRY           return 0;

   return sd->selector;
}

EAPI void
enna_list_remove_num(Evas_Object * obj, int n)
{
   E_Smart_Item       *si;

   API_ENTRY           return;

   if (!sd->items)
      return;
   si = evas_list_nth(sd->items, n);
   if (si)
     {
	sd->items = evas_list_remove(sd->items, si);
	if (enna_list_selected_get(obj) == n)
	   sd->selected = -1;
	if (si->icon_obj)
	   evas_object_del(si->icon_obj);
	evas_object_del(si->base_obj);
	free(si);
     }
}

EAPI void
enna_list_remove_label(Evas_Object * obj, const char *label)
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

	     t = strdup(edje_object_part_text_get
			(si->base_obj, "enna.text.label"));
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
enna_list_nth_label_get(Evas_Object * obj, int n)
{
   E_Smart_Item       *si;

   API_ENTRY           return NULL;

   if (!sd->items)
      return NULL;
   si = evas_list_nth(sd->items, n);
   if (si)
      return edje_object_part_text_get(si->base_obj, "enna.text.label");
   return NULL;
}

EAPI void
enna_list_nth_label_set(Evas_Object * obj, int n, const char *label)
{
   E_Smart_Item       *si;

   API_ENTRY           return;

   if (!sd->items)
      return;
   si = evas_list_nth(sd->items, n);
   if (si)
      edje_object_part_text_set(si->base_obj, "enna.text.label", label);
}

EAPI Evas_Object   *
enna_list_nth_icon_get(Evas_Object * obj, int n)
{
   E_Smart_Item       *si;

   API_ENTRY           return NULL;

   if (!sd->items)
      return NULL;
   si = evas_list_nth(sd->items, n);
   if (si)
      return si->icon_obj;
   return NULL;
}

EAPI void
enna_list_nth_icon_set(Evas_Object * obj, int n, Evas_Object * icon)
{
   E_Smart_Item       *si;

   API_ENTRY           return;

   if (!sd->items)
      return;
   si = evas_list_nth(sd->items, n);
   if (si)
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
	     edje_object_part_swallow(si->base_obj, "icon.swallow",
				      si->icon_obj);
	     evas_object_show(si->icon_obj);
	  }
     }
}

EAPI int
enna_list_count(Evas_Object * obj)
{
   API_ENTRY           return 0;

   return evas_list_count(sd->items);
}

EAPI void
enna_list_clear(Evas_Object * obj)
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
   sd->selected = -1;
}

EAPI int
enna_list_nth_is_header(Evas_Object * obj, int n)
{
   E_Smart_Item       *si;

   API_ENTRY           return 0;

   if (!sd->items)
      return 0;
   si = evas_list_nth(sd->items, n);
   if (si)
      return si->header;
   return 0;
}

EAPI void
enna_list_process_event(Evas_Object * obj, enna_event event)
{
   API_ENTRY           return;

   if (event == enna_event_up)
     {
	int                 n, ns;
	E_Smart_Item       *si;

	ns = enna_list_selected_get(sd->smart_obj);
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
	while ((si) && (si->header));
	if (n != ns)
	   enna_list_selected_set(sd->smart_obj, n);
     }
   else if (event == enna_event_down)
     {
	int                 n, ns;
	E_Smart_Item       *si;

	ns = enna_list_selected_get(sd->smart_obj);
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
	while ((si) && (si->header));
	if (n != ns)
	   enna_list_selected_set(sd->smart_obj, n);
     }
   else if (event == enna_event_home)
     {
	int                 n, ns;
	E_Smart_Item       *si;

	ns = enna_list_selected_get(sd->smart_obj);
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
	while ((si) && (si->header));
	if (n != ns)
	   enna_list_selected_set(sd->smart_obj, n);
     }
   else if (event == enna_event_end)
     {
	int                 n, ns;
	E_Smart_Item       *si;

	ns = enna_list_selected_get(sd->smart_obj);
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
	while ((si) && (si->header));
	if (n != ns)
	   enna_list_selected_set(sd->smart_obj, n);
     }
   else if (event == enna_event_enter)
     {
	E_Smart_Item       *si;

	si = evas_list_nth(sd->items, sd->selected);
	if (si)
	  {
	     if (si->func)
		si->func(si->data, si->data2);
	  }
     }
   else if (event >= enna_event_2 && event <= enna_event_9)
     {

	if (!sd->letter_mode)
	  {
	     char                letter[2];

	     sd->letter_mode = 1;
	     sd->letter_event_nbr = 0;
	     sd->letter_event = event;

	     if (event == enna_event_7)
		letter[0] = 'P';
	     else if (event == enna_event_8)
		letter[0] = 'T';
	     else if (event == enna_event_9)
		letter[0] = 'W';
	     else
		letter[0] = (event - 23) * 3 + 65;

	     letter[1] = '\0';
	     edje_object_signal_emit(sd->edje, "enna,letter,show", "enna");
	     edje_object_part_text_set(sd->edje, "enna.text.letter", letter);
	     sd->letter_timer = ecore_timer_add(TIME, _letter_timer_cb, sd);
	  }
	else
	  {
	     char                letter[2];
	     int                 mod = 3;

	     ecore_timer_del(sd->letter_timer);
	     sd->letter_mode = 1;

	     if (event == enna_event_7 || event == enna_event_9)
		mod = 4;
	     else
		mod = 3;

	     if (sd->letter_event == event)
		sd->letter_event_nbr = (sd->letter_event_nbr + 1) % mod;
	     else
	       {
		  sd->letter_event_nbr = 0;
		  sd->letter_event = event;
	       }

	     if (event == enna_event_7)
		letter[0] = 'P' + sd->letter_event_nbr;
	     else if (event == enna_event_8)
		letter[0] = 'T' + sd->letter_event_nbr;
	     else if (event == enna_event_9)
		letter[0] = 'W' + sd->letter_event_nbr;
	     else
		letter[0] = (event - 23) * 3 + 65 + sd->letter_event_nbr;

	     letter[1] = '\0';
	     edje_object_signal_emit(sd->edje, "enna,letter,show", "enna");
	     edje_object_part_text_set(sd->edje, "enna.text.letter", letter);
	     sd->letter_timer = ecore_timer_add(TIME, _letter_timer_cb, sd);

	  }

     }
}

/* local subsystem functions */
static int
_is_in_pl(E_Smart_Data * sd, char *filename)
{
   return 0;
}

static void
_select_down_event_mouse_out(void *data, Evas * e, Evas_Object * obj,
			     void *event_info)
{
   Evas_Event_Mouse_Out *ev;
   E_Smart_Data       *sd;

   sd = data;
   ev = event_info;
   if (sd)
     {
	if (sd->select_down_timer)
	  {
	     ecore_timer_del(sd->select_down_timer);
	     sd->select_down_timer = NULL;
	  }
     }
}

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
		  enna_list_selected_set_now(si->sd->smart_obj, i);
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
	   enna_list_selected_set_now(si->sd->smart_obj, i);
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
   Evas_List          *l;
   int                 i;

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
	/* FIXME : Go Back in browser */
     }
}

static void
_e_smart_event_mouse_wheel(void *data, Evas * e, Evas_Object * obj,
			   void *event_info)
{
   Evas_Event_Mouse_Wheel *ev;
   E_Smart_Item       *si;
   int                 i;

   si = data;
   ev = event_info;
   i = enna_list_selected_get(si->sd->smart_obj);
   enna_list_selected_set(si->sd->smart_obj, i + ev->z);
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
   char               *list_up, *list_down;
   char               *tmp;
   Evas_Object        *o_select_up, *o_select_down;

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
   sd->letter_mode = 0;
   sd->selected = -1;
   sd->box_scroll = NULL;
   sd->scroll_align = 0.0;
   sd->mouse_down = 0;
   sd->edje = edje_object_add(evas_object_evas_get(obj));
   edje_object_file_set(sd->edje, enna_config_theme_get(),
			"enna/list/container");
   if (!sd->edje)
      dbg("NOT EDJE\n");

   evas_object_show(sd->edje);

   list_up = (char *)edje_object_data_get(sd->edje, "select_up");
   if (list_up)
     {
	if (list_up[0] == '1')
	  {
	     edje_object_signal_emit(sd->edje, "enna,state,select_up,show",
				     "enna");
	     o_select_up = edje_object_part_object_get(sd->edje, "select_up");
	  }
	else
	   edje_object_signal_emit(sd->edje, "enna,state,select_up,hide",
				   "enna");
     }
   else
      dbg("Warning list_up data not defined in theme : /enna/list/container \n");

   list_down = (char *)edje_object_data_get(sd->edje, "select_down");
   if (list_down)
     {
	if (list_down[0] == '1')
	  {
	     edje_object_signal_emit(sd->edje, "enna,state,select_down,show",
				     "enna");
	     o_select_down =
		edje_object_part_object_get(sd->edje, "select_down");
	     evas_object_event_callback_add(o_select_down,
					    EVAS_CALLBACK_MOUSE_OUT,
					    _select_down_event_mouse_out, sd);
	     /*evas_object_event_callback_add(o_select_down,
	      * EVAS_CALLBACK_MOUSE_IN,
	      * _select_down_event_mouse_in, sd); */
	  }
	else
	   edje_object_signal_emit(sd->edje, "enna,state,select_down,hide",
				   "enna");
     }
   else
      dbg("Warning list_down data not defined in theme : /enna/list/container \n");

   tmp = (char *)edje_object_data_get(sd->edje, "nb_items_shown");
   if (!tmp)
      sd->nb_items_shown = 10;
   else
      sd->nb_items_shown = atoi(tmp);

   sd->select_up_timer = NULL;
   sd->select_down_timer = NULL;

   sd->box_obj = enna_box_add(evas_object_evas_get(obj));
   enna_box_align_set(sd->box_obj, 0.0, 0.0);
   enna_box_homogenous_set(sd->box_obj, 0);
   evas_object_smart_member_add(sd->edje, obj);
   edje_object_part_swallow(sd->edje, "enna.container.swallow", sd->box_obj);
   evas_object_propagate_events_set(obj, 0);
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
   evas_list_free(sd->items);
   evas_object_del(sd->letter);
   evas_object_del(sd->box_obj);
   evas_object_del(sd->edje);
   if (sd->box_scroll)
      ecore_timer_del(sd->box_scroll);
   if (sd->select_up_timer)
      ecore_timer_del(sd->select_up_timer);
   if (sd->select_down_timer)
      ecore_timer_del(sd->select_down_timer);
   if (sd->letter_timer)
      ecore_timer_del(sd->letter_timer);
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
   Evas_List          *l;
   int                 i;
   E_Smart_Item       *si;
   Evas_Coord          iw, ih;
   Evas_Coord          size = 0, s;

   if ((sd->w == w) && (sd->h == h))
      return;

   sd->w = w;
   sd->h = h;

   _e_smart_reconfigure(sd);

   if (!sd->items)
      return;

   iw = w;
   ih = h / sd->nb_items_shown;

   for (i = 0, l = sd->items; l; l = l->next, i++)
     {
	si = l->data;
	if (si)
	  {
	     enna_box_pack_options_set(si->base_obj, 1, 0,	/* fill */
				       1, 0,	/* expand */
				       0.5, 0.5,	/* align */
				       iw, ih,	/* min */
				       99999, 9999	/* max */
		);
	     evas_object_geometry_get(si->base_obj, NULL, NULL, NULL, &s);
	     size += s;
	  }
     }

   evas_object_geometry_get(sd->edje, NULL, NULL, NULL, &ih);

   if (size < ih)
      enna_box_align_set(sd->box_obj, 0.0, 0.5);
   else
      enna_box_align_set(sd->box_obj, 0.0, 1.0);

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
