#include "Etox_private.h"
#include "Etox.h"

void
_etox_get_string_width (Etox e, Etox_Font font, char *str, double *w)
{
  static Evas_Object text = NULL;

  if (!e || !font || !str || !w)
    return;

  if (!text)
    text = evas_add_text (e->evas, font->name, font->size, str);
  else
    {
      evas_set_text (e->evas, text, str);
      evas_set_font (e->evas, text, font->name, font->size);
    }

  evas_get_geometry (e->evas, text, NULL, NULL, w, NULL);
}

void
_etox_get_font_ascent_descent (Etox e, Etox_Font font,
			       double *ascent, double *descent)
{
  static Evas_Object text = NULL;

  if (!e || !font)
    return;

  if (!text)
    text = evas_add_text (e->evas, font->name, font->size, "");
  else
    evas_set_font (e->evas, text, font->name, font->size);

  evas_text_get_ascent_descent (e->evas, text, ascent, descent);
}

void
_etox_get_style_offsets (Etox_Style style, double *offset_w, double *offset_h)
{
  double max_x = 0.0, min_x = 0.0, max_y = 0.0, min_y = 0.0;
  Etox_Style_Bit style_bit;

  if (!style)
    return;

  /* get the width's offset caused by the style.. */
  ewd_list_goto_first (style->bits);
  while ((style_bit = (Etox_Style_Bit) ewd_list_next (style->bits)))
    {
      if (style_bit->x > max_x)
	max_x = style_bit->x;
      if (style_bit->x < min_x)
	min_x = style_bit->x;
      if (style_bit->y > max_y)
	max_y = style_bit->y;
      if (style_bit->y < min_y)
	min_y = style_bit->y;
    }

  if (offset_w)
    *offset_w = max_x - min_x;
  if (offset_h)
    *offset_h = max_y - min_y;
}


Evas
etox_get_evas (Etox e)
{
  if (!e)
    return NULL;

  return e->evas;
}

char *
etox_get_name (Etox e)
{
  if (!e)
    return NULL;

  return e->name;
}

int
etox_get_alpha (Etox e)
{
  if (!e)
    return 0;

  return e->a;
}

double
etox_get_padding (Etox e)
{
  if (!e)
    return 0.0;

  return e->padding;
}

int
etox_get_layer (Etox e)
{
  if (!e)
    return 0;

  return e->layer;
}

Evas_Object
etox_get_clip (Etox e)
{
  if (!e)
    return NULL;

  return e->clip;
}

Etox_Align_Type
etox_get_align_v (Etox e)
{
  if (!e || !e->def.align)
    return ETOX_ALIGN_TYPE_NULL;

  return e->def.align->v;
}

Etox_Align_Type
etox_get_align_h (Etox e)
{
  if (!e || !e->def.align)
    return ETOX_ALIGN_TYPE_NULL;

  return e->def.align->h;
}

Etox_Color
etox_get_color (Etox e)
{
  if (!e)
    return NULL;

  return e->def.color;
}

char *
etox_get_font_name (Etox e)
{
  if (!e || !e->def.font)
    return NULL;

  return e->def.font->name;
}

int
etox_get_font_size (Etox e)
{
  if (!e || e->def.font)
    return 0;

  return e->def.font->size;
}

Etox_Style
etox_get_style (Etox e)
{
  if (!e)
    return NULL;

  return e->def.style;
}

int
etox_get_text_string_lenght (Etox e)
{
  Etox_Bit bit;
  Etox_Text text;
  int size = 0;

  if (!e || !e->bits || ewd_list_is_empty (e->bits))
    return -1;

  ewd_list_goto_first (e->bits);
  while ((bit = (Etox_Bit) ewd_list_next (e->bits)))
    if (bit->type == ETOX_BIT_TYPE_TEXT)
      {
	text = (Etox_Text) bit->body;
	size += strlen (text->str);
      }

  return size;
}

char *
etox_get_text_string (Etox e)
{
  Etox_Bit bit;
  Etox_Text text;
  char *p;
  int size = 0;

  if (!e || !e->bits || ewd_list_is_empty (e->bits))
    return NULL;

  if (!(size = etox_get_text_string_lenght (e)))
    return NULL;

  p = (char *) malloc ((sizeof (char) * size) + 1);

  ewd_list_goto_first (e->bits);
  while ((bit = (Etox_Bit) ewd_list_next (e->bits)))
    if (bit->type == ETOX_BIT_TYPE_TEXT)
      {
	text = (Etox_Text) bit->body;
	strcat (p, text->str);
      }

  return p;
}

int
etox_get_actual_text_string_lenght (Etox e)
{
  Etox_Object obj;
  Etox_Object_Bit obj_bit;
  Etox_Object_String obj_str;
  int size = 0;

  if (!e || !e->etox_objects.list || ewd_list_is_empty (e->etox_objects.list))
    return -1;

  ewd_list_goto_first (e->etox_objects.list);
  while ((obj = (Etox_Object) ewd_list_next (e->etox_objects.list)))
    if (obj->bits && !ewd_list_is_empty (obj->bits))
      {
	ewd_list_goto_first (obj->bits);
	while ((obj_bit = (Etox_Object_Bit) ewd_list_next (obj->bits)))
	  if (obj_bit->type == ETOX_OBJECT_BIT_TYPE_STRING)
	    {
	      obj_str = (Etox_Object_String) obj_bit->body;
	      size += strlen (obj_str->str);
	    }
      }

  return size;
}

char *
etox_get_actual_text_string (Etox e)
{
  Etox_Object obj;
  Etox_Object_Bit obj_bit;
  Etox_Object_String obj_str;
  char *p = NULL;
  int size = 0.0;

  if (!e || !e->etox_objects.list || ewd_list_is_empty (e->etox_objects.list))
    return NULL;

  if (!(size = etox_get_actual_text_string_lenght (e)))
    return NULL;

  p = (char *) malloc ((sizeof (char) * size) + 1);

  ewd_list_goto_first (e->etox_objects.list);
  while ((obj = (Etox_Object) ewd_list_next (e->etox_objects.list)))
    if (obj->bits && !ewd_list_is_empty (obj->bits))
      {
	ewd_list_goto_first (obj->bits);
	while ((obj_bit = (Etox_Object_Bit) ewd_list_next (obj->bits)))
	  if (obj_bit->type == ETOX_OBJECT_BIT_TYPE_STRING)
	    {
	      obj_str = (Etox_Object_String) obj_bit->body;
	      strcat (p, obj_str->str);
	    }
      }

  return p;
}

void
etox_get_geometry (Etox e, double *x, double *y, double *w, double *h)
{
  if (!e)
    return;

  if (x)
    *x = e->x;
  if (y)
    *y = e->y;
  if (w)
    *w = e->w;
  if (h)
    *h = e->h;
}

void
etox_get_actual_geometry (Etox e, double *x, double *y, double *w, double *h)
{
  Etox_Object obj, first_obj = NULL, last_obj = NULL;
  Etox_Object_Bit obj_bit;
  double left_x = 0.0, right_x = 0.0, my_y = 0.0;
  int first_check = 1;

  if (!e || !e->etox_objects.list || ewd_list_is_empty (e->etox_objects.list))
    return;

  if (x)
    *x = 0.0;
  if (y)
    *y = 0.0;
  if (w)
    *w = 0.0;
  if (h)
    *h = 0.0;

  if (w)
    {
      left_x = e->x + e->w;
      right_x = e->x;
    }

  ewd_list_goto_first (e->etox_objects.list);
  while ((obj = (Etox_Object) ewd_list_next (e->etox_objects.list)))
    if (obj->bits && !ewd_list_is_empty (obj->bits))
      {
	if (first_check)
	  {
	    first_obj = obj;
	    first_check = 0;
	  }
	if (x || w)
	  {
	    obj_bit = (Etox_Object_Bit) ewd_list_goto_first (obj->bits);
	    if (obj_bit->x < left_x)
	      left_x = obj_bit->x;
	    obj_bit = (Etox_Object_Bit) ewd_list_goto_last (obj->bits);
	    if ((obj_bit->x + obj_bit->w) > right_x)
	      right_x = obj_bit->x + obj_bit->w;
	  }
	last_obj = obj;
      }

  if (x)
    *x = ET_X_TO_EV (left_x);
  if (y)
    {
      obj_bit = (Etox_Object_Bit) ewd_list_goto_first (first_obj->bits);
      *y = ET_Y_TO_EV (obj_bit->y);
    }
  if (w)
    *w = right_x - left_x;
  if (h)
    {
      if (!y)
	{
	  obj_bit = (Etox_Object_Bit) ewd_list_goto_first (first_obj->bits);
	  my_y = ET_Y_TO_EV (obj_bit->y);
	}
      else
	my_y = *y;
      obj_bit = (Etox_Object_Bit) ewd_list_goto_first (last_obj->bits);
      *h = (obj_bit->y + obj_bit->h) - EV_Y_TO_ET (my_y);
    }
}

void
etox_get_char_geometry_at (Etox e, int index,
			   double *char_x, double *char_y,
			   double *char_w, double *char_h)
{
  Etox_Object obj = NULL;
  Etox_Object_Bit obj_bit = NULL;
  Etox_Object_String obj_str = NULL;
  int my_index = 0;

  if (!e || (index < 0) || !e->etox_objects.list ||
      ewd_list_is_empty (e->etox_objects.list))
    return;

  if (char_x)
    *char_x = 0.0;
  if (char_y)
    *char_y = 0.0;
  if (char_w)
    *char_w = 0.0;
  if (char_h)
    *char_h = 0.0;

  ewd_list_goto_first (e->etox_objects.list);
  while ((obj = (Etox_Object) ewd_list_next (e->etox_objects.list)))
    if (obj->bits && !ewd_list_is_empty (obj->bits))
      {
	ewd_list_goto_first (obj->bits);
	while ((obj_bit = (Etox_Object_Bit) ewd_list_next (obj->bits)))
	  if (obj_bit->type == ETOX_OBJECT_BIT_TYPE_STRING)
	    {
	      obj_str = (Etox_Object_String) obj_bit->body;
	      my_index += strlen (obj_str->str);
	      if (my_index >= index)
		{
		  my_index -= strlen (obj_str->str);
		  my_index = index - my_index;
		  _etox_object_bit_get_char_geometry_at (e, obj_bit,
							 my_index,
							 char_x,
							 char_y,
							 char_w, char_h);
		  return;
		}
	    }
      }
}

int
etox_get_char_geometry_at_position (Etox e, double x, double y,
				    double *char_x, double *char_y,
				    double *char_w, double *char_h)
{
  Etox_Object obj = NULL;
  Etox_Object_Bit obj_bit = NULL;
  Etox_Object_String obj_str = NULL;
  double my_x = 0.0, my_y = 0.0;
  int index = 0;

  if (!e || !e->etox_objects.list || ewd_list_is_empty (e->etox_objects.list))
    return -2;

  if (char_x)
    *char_x = 0.0;
  if (char_y)
    *char_y = 0.0;
  if (char_w)
    *char_w = 0.0;
  if (char_h)
    *char_h = 0.0;

  ewd_list_goto_first (e->etox_objects.list);
  while ((obj = (Etox_Object) ewd_list_next (e->etox_objects.list)))
    if (obj->bits && !ewd_list_is_empty (obj->bits))
      {
	ewd_list_goto_first (obj->bits);
	while ((obj_bit = (Etox_Object_Bit) ewd_list_next (obj->bits)))
	  if (obj_bit->type == ETOX_OBJECT_BIT_TYPE_STRING)
	    {
	      if ((obj_bit->x <= x) && ((obj_bit->x + obj_bit->w) >= x) &&
		  (obj_bit->y <= y) && ((obj_bit->y + obj_bit->h) >= y))
		{
		  my_x = x - obj_bit->x;
		  my_y = y - obj_bit->y;

		  index += _etox_object_bit_get_char_geometry_at_position (e,
									   obj_bit,
									   my_x,
									   my_y,
									   char_x,
									   char_y,
									   char_w,
									   char_h);
		  return index;
		}

	      obj_str = (Etox_Object_String) obj_bit->body;
	      index += strlen (obj_str->str);
	    }
      }

  return -2;
}
