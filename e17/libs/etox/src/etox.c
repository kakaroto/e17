#include "Etox_private.h"
#include "Etox.h"

void
#ifdef DEBUG
_D_etox_update (Etox e)
#else
_etox_update (Etox e)
#endif
{
  Evas_Object ev_obj;

  if (e->etox_objects.bits.dirty)
    _etox_create_etox_object_bits (e);

  if (e->etox_objects.dirty)
    _etox_create_etox_objects (e);
  if (e->evas_objects.dirty)
    _etox_create_evas_objects (e);

  if (!e->evas_objects.list || ewd_list_is_empty (e->evas_objects.list))
    return;

  ewd_list_goto_first (e->evas_objects.list);
  while ((ev_obj = (Evas_Object) ewd_list_next (e->evas_objects.list)))
    {
      if (e->show)
	evas_show (e->evas, ev_obj);
      else
	evas_hide (e->evas, ev_obj);
      if (e->raise)
	evas_raise (e->evas, ev_obj);
      if (e->lower)
	evas_lower (e->evas, ev_obj);
      if (e->layer)
	evas_set_layer (e->evas, ev_obj, e->layer);
      if (e->clip)
	evas_set_clip (e->evas, ev_obj, e->clip);
    }
}

Etox
etox_new (Evas evas, char *name)
{

  Etox e;
  Etox_Style_Bit style_bit;

  if (!evas)
    return NULL;

  etox_style_add_path (".");

  e = malloc (sizeof (struct _Etox));
  e->evas = evas;
  if (name)
    e->name = strdup (name);
  else
    e->name = NULL;

  e->x = 0.0;
  e->y = 0.0;
  e->w = 100.0;
  e->h = 100.0;
  e->a = 255;
  e->padding = 7.0;

  e->show = 1;
  e->raise = 0;
  e->lower = 0;
  e->layer = 0;
  e->clip = NULL;

  e->def.align = malloc (sizeof (struct _Etox_Align));
  e->def.align->v = ETOX_ALIGN_TYPE_TOP;
  e->def.align->h = ETOX_ALIGN_TYPE_LEFT;

  /* FIXME: add callback */
  e->def.callback = NULL;

  e->def.color = etox_color_new ();
  etox_color_set_member (e->def.color, "sh", 0, 0, 0, 255);
  etox_color_set_member (e->def.color, "ol", 0, 0, 0, 255);
  etox_color_set_member (e->def.color, "fg", 255, 255, 255, 255);

  e->def.font = malloc (sizeof (struct _Etox_Font));
  e->def.font->name = strdup ("notepad");
  e->def.font->size = 12;
  _etox_get_font_ascent_descent (e, e->def.font,
				 &(e->def.font->ascent),
				 &(e->def.font->descent));

  e->def.style = malloc (sizeof (struct _Etox_Style));
  e->def.style->name = strdup ("Default");
  e->def.style->bits = ewd_list_new ();
  ewd_list_set_free_cb (e->def.style->bits, EWD_FREE_CB (free));
  style_bit = malloc (sizeof (struct _Etox_Style_Bit));
  style_bit->type = ETOX_STYLE_TYPE_SHADOW;
  style_bit->x = 2;
  style_bit->y = 2;
  style_bit->a = 128;
  ewd_list_append (e->def.style->bits, style_bit);
  style_bit = malloc (sizeof (struct _Etox_Style_Bit));
  style_bit->type = ETOX_STYLE_TYPE_OUTLINE;
  style_bit->x = 0;
  style_bit->y = -1;
  style_bit->a = 255;
  ewd_list_append (e->def.style->bits, style_bit);
  style_bit = malloc (sizeof (struct _Etox_Style_Bit));
  style_bit->type = ETOX_STYLE_TYPE_OUTLINE;
  style_bit->x = 1;
  style_bit->y = 0;
  style_bit->a = 255;
  ewd_list_append (e->def.style->bits, style_bit);
  style_bit = malloc (sizeof (struct _Etox_Style_Bit));
  style_bit->type = ETOX_STYLE_TYPE_OUTLINE;
  style_bit->x = 0;
  style_bit->y = 1;
  style_bit->a = 255;
  ewd_list_append (e->def.style->bits, style_bit);
  style_bit = malloc (sizeof (struct _Etox_Style_Bit));
  style_bit->type = ETOX_STYLE_TYPE_FOREGROUND;
  style_bit->x = 0;
  style_bit->y = 0;
  style_bit->a = 255;
  ewd_list_append (e->def.style->bits, style_bit);

  _etox_get_style_offsets (e->def.style,
			   &(e->def.style->offset_w),
			   &(e->def.style->offset_h));

  e->def.text = malloc (sizeof (struct _Etox_Text));
  e->def.text->str = strdup ("Empty string");

  e->bits = NULL;
  e->obstacles = NULL;

  e->etox_objects.list = NULL;
  e->etox_objects.h = 0.0;
  e->etox_objects.dirty = 1;
  e->etox_objects.bits.list = NULL;
  e->etox_objects.bits.dirty = 1;

  e->evas_objects.list = NULL;
  e->evas_objects.dirty = 1;

  return e;
}

Etox
etox_new_all (Evas evas, char *name,
	      double x, double y, double w, double h,
	      int alpha, double padding,
	      Etox_Align_Type v_align, Etox_Align_Type h_align,
	      Etox_Callback callback, Etox_Color color,
	      char *font, int font_size, Etox_Style style)
{
  Etox e;

  if (!evas || !w || !h)
    return NULL;

  e = etox_new (evas, name);
  etox_move (e, x, y);
  etox_resize (e, w, h);
  etox_set_alpha (e, alpha);
  etox_set_padding (e, padding);
  if (h_align && v_align)
    etox_set_align (e, h_align, v_align);
  /* FIXME: add callback */
  if (color)
    etox_set_color (e, color);
  if (font && font_size)
    etox_set_font (e, font, font_size);
  if (style)
    etox_set_style (e, style);

  return e;
}

void
etox_free (Etox e)
{
  IF_FREE (e->name);
  IF_FREE (e->def.align);
  IF_FREE (e->def.callback);
  if (e->def.color)
    etox_color_free (e->def.color);
  if (e->def.font)
    {
      IF_FREE (e->def.font->name);
      FREE (e->def.font);
    }
  IF_FREE (e->def.style);
  IF_FREE (e->def.text);
  if (e->bits)
    ewd_list_destroy (e->bits);
  if (e->obstacles)
    ewd_list_destroy (e->obstacles);
  if (e->etox_objects.list)
    ewd_list_destroy (e->etox_objects.list);
  if (e->etox_objects.bits.list)
    ewd_list_destroy (e->etox_objects.bits.list);
  if (e->evas_objects.list)
    ewd_list_destroy (e->evas_objects.list);
}

void
etox_show (Etox e)
{
  if (!e)
    return;

  e->show = 1;

  _etox_update (e);
}

void
etox_hide (Etox e)
{
  if (!e)
    return;

  e->show = 0;

  _etox_update (e);
}

void
etox_raise (Etox e)
{
  if (!e)
    return;

  e->raise = 1;

  _etox_update (e);
}

void
etox_lower (Etox e)
{
  if (!e)
    return;

  e->lower = 1;

  _etox_update (e);
}

void
etox_move (Etox e, double x, double y)
{
  if (!e)
    return;

  e->x = x;
  e->y = y;

  e->etox_objects.dirty = 1;
  _etox_update (e);
}

void
etox_resize (Etox e, double w, double h)
{
  if (!e)
    return;

  e->w = w;
  e->h = h;

  e->etox_objects.dirty = 1;
  _etox_update (e);
}
