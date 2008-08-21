/*
 * enna_listitem.c
 * Copyright (C) Nicolas Aguirre 2006,2007,2008 <aguirre.nicolas@gmail.com>
 *
 * enna_listitem.c is free software copyrighted by Nicolas Aguirre.
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
 * enna_listitem.c IS PROVIDED BY Nicolas Aguirre ``AS IS'' AND ANY EXPRESS
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

/* derived from e_icon */

#include "list_item.h"
#include "enna.h"

#define SMART_NAME "enna_listitem"

#define API_ENTRY \
   E_Smart_Data *sd; \
   sd = evas_object_smart_data_get(obj); \
   if ((!obj) || (!sd) || \
      (evas_object_type_get(obj) \
      && strcmp(evas_object_type_get(obj), SMART_NAME)))

#define INTERNAL_ENTRY \
   E_Smart_Data *sd; \
   sd = evas_object_smart_data_get(obj); \
   if (!sd) return;

typedef struct _E_Smart_Data E_Smart_Data;

struct _E_Smart_Data
{
   Evas_Coord    x, y, w, h, iw, ih;
   Evas_Object  *o_edje;
   Evas_Object  *o_icon;
   unsigned char full: 1;
};

/* local subsystem functions */
static void         _enna_listitem_smart_reconfigure(E_Smart_Data * sd);
static void         _enna_listitem_smart_init(void);
static void         _e_smart_add(Evas_Object * obj);
static void         _e_smart_del(Evas_Object * obj);
static void         _e_smart_move(Evas_Object * obj, Evas_Coord x,
				  Evas_Coord y);
static void         _e_smart_resize(Evas_Object * obj, Evas_Coord w,
				    Evas_Coord h);
static void         _e_smart_show(Evas_Object * obj);
static void         _e_smart_hide(Evas_Object * obj);
static void         _e_smart_color_set(Evas_Object * obj, int r, int g,
				       int b, int a);
static void         _e_smart_clip_set(Evas_Object * obj, Evas_Object * clip);
static void         _e_smart_clip_unset(Evas_Object * obj);

/* local subsystem globals */
static Evas_Smart  *_e_smart = NULL;

/* externally accessible functions */
EAPI Evas_Object   *
enna_listitem_add(Evas * evas)
{
   _enna_listitem_smart_init();
   return evas_object_smart_add(evas, _e_smart);
}

EAPI void
enna_listitem_create_simple( Evas_Object *obj, Evas_Object *icon,
			     const char *label)
{
   API_ENTRY return;
   if (sd->o_edje) return;

   sd->o_edje = edje_object_add(evas_object_evas_get(obj));
   edje_object_file_set(sd->o_edje, enna_config_theme_get(),
			"enna/list/item");
   sd->o_icon = icon;
   if (sd->o_icon)
     {
	edje_extern_object_min_size_set(sd->o_icon, sd->iw, sd->ih);
	edje_object_part_swallow(sd->o_edje, "enna.swallow.icon", sd->o_icon);
	evas_object_show(sd->o_icon);
     }
   edje_object_part_text_set(sd->o_edje, "enna.text.label", label);
   evas_object_smart_member_add(sd->o_edje, obj);
   sd->full = 0;
}

EAPI void
enna_listitem_create_full( Evas_Object *obj,  Evas_Object *icon,
			   const char *info,  const char *title,
			   const char *album, const char *artist)
{
   API_ENTRY return;
   if (sd->o_edje) return;
   sd->o_edje = edje_object_add(evas_object_evas_get(obj));
   edje_object_file_set(sd->o_edje, enna_config_theme_get(),
			"enna/list/item_full");
   sd->o_icon = icon;
   if (sd->o_icon)
     {
	edje_extern_object_min_size_set(sd->o_icon, 100, 100);
	edje_object_part_swallow(sd->o_edje, "enna.swallow.icon", sd->o_icon);
	evas_object_show(sd->o_icon);
     }
   edje_object_part_text_set(sd->o_edje, "enna.text.info", info ? info : "");
   edje_object_part_text_set(sd->o_edje, "enna.text.title",title ? title : "Unknown");
   edje_object_part_text_set(sd->o_edje, "enna.text.album", album ? album : "Unknown Album");
   edje_object_part_text_set(sd->o_edje, "enna.text.artist", artist ? artist : "Unknown Artist");
   evas_object_smart_member_add(sd->o_edje, obj);
   sd->full = 1;
}


EAPI void
enna_listitem_min_size_get(Evas_Object *obj, Evas_Coord *w, Evas_Coord *h)
{
   API_ENTRY return;
   if (!sd->full)
     {
	if(w) *w = 100;
	if(h) *h = 100;
	//edje_object_size_min_calc(sd->o_edje, w, h);
     }
   else
     {
	if(w) *w = 150;
	if(h) *h = 150;
     }

}

EAPI const char *
enna_listitem_label_get(Evas_Object *obj)
{
  API_ENTRY return NULL;
  if (sd->full)
    return  edje_object_part_text_get(sd->o_edje, "enna.text.info");
  else
   return edje_object_part_text_get(sd->o_edje, "enna.text.label");

}

EAPI void
enna_listitem_select(Evas_Object *obj)
{
   API_ENTRY return;
   edje_object_signal_emit(sd->o_edje, "enna,state,selected", "enna");
}

EAPI void
enna_listitem_unselect(Evas_Object *obj)
{
   API_ENTRY return;
   edje_object_signal_emit(sd->o_edje, "enna,state,unselected", "enna");
}

EAPI void
enna_listitem_min_size_set(Evas_Object *obj, Evas_Coord w, Evas_Coord h)
{
   API_ENTRY return;
   if ((sd->iw == w) && (sd->ih == h)) return;
   sd->iw = w;
   sd->ih = h;
   if (!sd->o_icon) return;
   edje_extern_object_min_size_set(sd->o_icon, w, h);
   edje_object_part_swallow(sd->o_edje, "enna.swallow.icon", sd->o_icon);

}

/* local subsystem globals */
static void
_enna_listitem_smart_reconfigure(E_Smart_Data * sd)
{
   Evas_Coord          x, y, w, h;

   x = sd->x;
   y = sd->y;
   w = sd->w;
   h = sd->h;

   evas_object_move(sd->o_edje, x, y);
   evas_object_resize(sd->o_edje, w, h);

}

static void
_enna_listitem_smart_init(void)
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

static void
_e_smart_add(Evas_Object * obj)
{
   E_Smart_Data       *sd;

   sd = calloc(1, sizeof(E_Smart_Data));
   if (!sd)
      return;
   sd->x = 0;
   sd->y = 0;
   sd->w = 0;
   sd->h = 0;
   sd->iw = sd->ih = 48;
   evas_object_smart_data_set(obj, sd);
}

static void
_e_smart_del(Evas_Object * obj)
{
   E_Smart_Data       *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd)
      return;
   evas_object_del(sd->o_edje);
   evas_object_del(sd->o_icon);
   free(sd);
}

static void
_e_smart_move(Evas_Object * obj, Evas_Coord x, Evas_Coord y)
{
   E_Smart_Data       *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd)
      return;
   if ((sd->x == x) && (sd->y == y))
      return;
   sd->x = x;
   sd->y = y;
   _enna_listitem_smart_reconfigure(sd);
}

static void
_e_smart_resize(Evas_Object * obj, Evas_Coord w, Evas_Coord h)
{
   E_Smart_Data       *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd)
      return;
   if ((sd->w == w) && (sd->h == h))
      return;
   sd->w = w;
   sd->h = h;
   _enna_listitem_smart_reconfigure(sd);
}

static void
_e_smart_show(Evas_Object * obj)
{
   E_Smart_Data       *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd)
      return;
   evas_object_show(sd->o_edje);
}

static void
_e_smart_hide(Evas_Object * obj)
{
   E_Smart_Data       *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd)
      return;
   evas_object_hide(sd->o_edje);
}

static void
_e_smart_color_set(Evas_Object * obj, int r, int g, int b, int a)
{
   E_Smart_Data       *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd)
      return;
   evas_object_color_set(sd->o_edje, r, g, b, a);
}

static void
_e_smart_clip_set(Evas_Object * obj, Evas_Object * clip)
{
   E_Smart_Data       *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd)
      return;
   evas_object_clip_set(sd->o_edje, clip);
}

static void
_e_smart_clip_unset(Evas_Object * obj)
{
   E_Smart_Data       *sd;

   sd = evas_object_smart_data_get(obj);
   if (!sd)
      return;
   evas_object_clip_unset(sd->o_edje);
}
