#include "Etox_private.h"
#include "Etox.h"

static void
__get_string_size(Etox e, char *str, Etox_Font font, Etox_Style style, 
                  double *w, double *h)
{
  if (!e || !str || !style)
    return;

  _etox_get_string_width(e, font, str, w);
  *w += style->offset_w;
  *h = font->ascent + font->descent + style->offset_h;
}

Etox_Object
_etox_object_new(Etox e, double x, double y)
{
  Etox_Object obj;

  if (!e)
    return NULL;

  obj = malloc(sizeof(struct _Etox_Object));
  obj->x = x;
  obj->y = y;
  obj->w = 0.0;
  obj->bits = NULL;

  return obj;  
}

void
_etox_object_free(Etox_Object obj)
{
  if (!obj)
    return;

  IF_FREE(obj);
}

int
_etox_object_get_available_size(Etox_Object obj, double *w)
{
  Etox_Object_Bit bit;
  double used_w = 0.0;

  if (!obj)
    return 0;

  if (!obj->bits || ewd_list_is_empty(obj->bits))
    {
      *w = obj->w;
      return 1;
    }

  if ((bit = (Etox_Object_Bit) ewd_list_goto_last(obj->bits)))
    if (bit->type == ETOX_OBJECT_BIT_TYPE_NEWLINE)
      return 0;

  ewd_list_goto_first(obj->bits);
  while ((bit = (Etox_Object_Bit) ewd_list_next(obj->bits)))
    used_w += bit->w; 

  if (used_w >= obj->w)
    return 0;

  *w = obj->w - used_w;

  return 1;
}

int
_etox_object_add_bit(Etox_Object obj, Etox_Object_Bit obj_bit) 
{
  double w;

  if (!obj || !obj_bit)
    return 0;

  if (!_etox_object_get_available_size(obj, &w))
    return 0;

  if (obj_bit->w > w)
    return 0;

  if (!obj->bits)
    {
      obj->bits = ewd_list_new();
      ewd_list_set_free_cb(obj->bits, EWD_FREE_CB(_etox_object_bit_free));
    }

  ewd_list_append(obj->bits, obj_bit);

  return 1;
}

Etox_Object_Bit
_etox_object_bit_new(void)
{
  Etox_Object_Bit bit;

  bit = (Etox_Object_Bit) malloc(sizeof(struct _Etox_Object_Bit));
  bit->type = ETOX_OBJECT_BIT_TYPE_NULL;
  bit->w = 0.0;
  bit->h = 0.0;
  bit->body = NULL;
  bit->evas_objects_list.fg = NULL;
  bit->evas_objects_list.sh = NULL;
  bit->evas_objects_list.ol = NULL;

  return bit;
}

void
_etox_object_bit_free(Etox_Object_Bit bit)
{
  if (!bit)
    return;

  switch (bit->type)
    {
    case ETOX_OBJECT_BIT_TYPE_STRING:
      _etox_object_string_free((Etox_Object_String) bit->body);
      break;
    case ETOX_OBJECT_BIT_TYPE_NEWLINE:
      _etox_object_newline_free((Etox_Object_Newline) bit->body);
      break;
    case ETOX_OBJECT_BIT_TYPE_TAB:
      _etox_object_tab_free((Etox_Object_Tab) bit->body);
      break;
    default:
      D_PRINT("Unkown Etox_Object_Bit, not freed!\n");
      break;
    }

  if (bit->evas_objects_list.fg)
    ewd_list_destroy(bit->evas_objects_list.fg);
  if (bit->evas_objects_list.sh)
    ewd_list_destroy(bit->evas_objects_list.sh);
  if (bit->evas_objects_list.ol)
    ewd_list_destroy(bit->evas_objects_list.ol);

  FREE(bit);  
}

void
_etox_object_bit_set_body(Etox e, Etox_Object_Bit bit, void *body,
                          Etox_Object_Bit_Type type)
{
  if (!bit || !body)
    return;

  bit->body = body;
  bit->type = type;

  switch (type)
    {
    case ETOX_OBJECT_BIT_TYPE_STRING:
      {
        Etox_Object_String string;
        
        string = (Etox_Object_String) body;
        __get_string_size(e, string->str, string->font, string->style,
                          &bit->w, &bit->h);
        break;
      }
    case ETOX_OBJECT_BIT_TYPE_NEWLINE:
    default:
      bit->w = 0.0;
      bit->h = 0.0;
      break;
    case ETOX_OBJECT_BIT_TYPE_TAB:
      {
        Etox_Object_Tab tab;

        tab = (Etox_Object_Tab) body;
        __get_string_size(e, "    ", tab->font, e->def.style,
                        &bit->w, &bit->h);
        break;
      }
    }
}

void
_etox_object_bit_get_char_geometry_at(Etox e,
                                      Etox_Object_Bit obj_bit,
                                      int index,
                                      double *char_x, double *char_y,
                                      double *char_w, double *char_h)
{
  Evas_Object ev_obj;
  Etox_Object_String obj_str = NULL;

  /* FIXME: this func needs rethinking.. */

  if ( !e || !obj_bit || (obj_bit->type != ETOX_OBJECT_BIT_TYPE_STRING) ||
       ( (!obj_bit->evas_objects_list.fg || 
          ewd_list_is_empty(obj_bit->evas_objects_list.fg)) && 
         (!obj_bit->evas_objects_list.sh || 
          ewd_list_is_empty(obj_bit->evas_objects_list.sh)) &&
         (!obj_bit->evas_objects_list.ol || 
          ewd_list_is_empty(obj_bit->evas_objects_list.ol)) ) )
    return;

  if (obj_bit->evas_objects_list.fg)
    ewd_list_goto_first(obj_bit->evas_objects_list.fg);
  if (obj_bit->evas_objects_list.sh)
    ewd_list_goto_first(obj_bit->evas_objects_list.sh);
  if (obj_bit->evas_objects_list.ol)
    ewd_list_goto_first(obj_bit->evas_objects_list.ol);

  if (obj_bit->evas_objects_list.fg && 
      !ewd_list_is_empty(obj_bit->evas_objects_list.fg))
    {
      ev_obj = (Evas_Object) ewd_list_next(obj_bit->evas_objects_list.fg);
      evas_text_at(e->evas, ev_obj, index, char_x, char_y, char_w, char_h);
    }
  else
  if (obj_bit->evas_objects_list.sh && 
      !ewd_list_is_empty(obj_bit->evas_objects_list.sh))
    {
      ev_obj = (Evas_Object) ewd_list_next(obj_bit->evas_objects_list.sh);
      evas_text_at(e->evas, ev_obj, index, char_x, char_y, char_w, char_h);
    }
  else
  if (obj_bit->evas_objects_list.ol && 
      !ewd_list_is_empty(obj_bit->evas_objects_list.ol))
    {
      ev_obj = (Evas_Object) ewd_list_next(obj_bit->evas_objects_list.ol);
      evas_text_at(e->evas, ev_obj, index, char_x, char_y, char_w, char_h);
    } 

  if (char_w || char_h)
    obj_str = (Etox_Object_String) obj_bit->body;

  if (char_x)
    *char_x += obj_bit->x;
  if (char_y)
    *char_y += obj_bit->y;

  if (char_w)
    *char_w += obj_str->style->offset_w;
  if (char_h)
    *char_h += obj_str->style->offset_h;
}

int
_etox_object_bit_get_char_geometry_at_position(Etox e,
                                               Etox_Object_Bit obj_bit,
                                               double x, double y,
                                               double *char_x, 
                                               double *char_y,
                                               double *char_w, 
                                               double *char_h)
{
  Evas_Object ev_obj = NULL;
  Etox_Object_String obj_str = NULL;
  int index = -2;

  /* FIXME: this func needs rethinking.. */

  if ( !e || !obj_bit || (obj_bit->type != ETOX_OBJECT_BIT_TYPE_STRING) ||
       ( (!obj_bit->evas_objects_list.fg ||
          ewd_list_is_empty(obj_bit->evas_objects_list.fg)) &&
         (!obj_bit->evas_objects_list.sh ||
          ewd_list_is_empty(obj_bit->evas_objects_list.sh)) &&
         (!obj_bit->evas_objects_list.ol ||
          ewd_list_is_empty(obj_bit->evas_objects_list.ol)) ) )
    return -2;

  if (obj_bit->evas_objects_list.fg)
    ewd_list_goto_first(obj_bit->evas_objects_list.fg);
  if (obj_bit->evas_objects_list.sh)
    ewd_list_goto_first(obj_bit->evas_objects_list.sh);
  if (obj_bit->evas_objects_list.ol)
    ewd_list_goto_first(obj_bit->evas_objects_list.ol);

  if (obj_bit->evas_objects_list.fg &&
      !ewd_list_is_empty(obj_bit->evas_objects_list.fg))
    {
      ev_obj = (Evas_Object) ewd_list_next(obj_bit->evas_objects_list.fg);
      index = evas_text_at_position(e->evas, ev_obj, x, y, char_x, char_y, 
                                    char_w, char_h);
    }
  else
  if (obj_bit->evas_objects_list.sh && 
      !ewd_list_is_empty(obj_bit->evas_objects_list.sh))
    {  
      ev_obj = (Evas_Object) ewd_list_next(obj_bit->evas_objects_list.sh);
      index = evas_text_at_position(e->evas, ev_obj, x, y, 
                                    char_x, char_y, char_w, char_h);
    }
  else
  if (obj_bit->evas_objects_list.ol &&
      !ewd_list_is_empty(obj_bit->evas_objects_list.ol))
    {
      ev_obj = (Evas_Object) ewd_list_next(obj_bit->evas_objects_list.ol);
      index = evas_text_at_position(e->evas, ev_obj, x, y, 
                                    char_x, char_y, char_w, char_h);
    }

  if (char_w || char_h)
    obj_str = (Etox_Object_String) obj_bit->body;

  if (char_x)
    *char_x += obj_bit->x;
  if (char_y)
    *char_y += obj_bit->y;

  if (char_w)
    *char_w += obj_str->style->offset_w;
  if (char_h)
    *char_h += obj_str->style->offset_h;    

  return index;
}

Etox_Object_String
_etox_object_string_new(char *str, Etox_Align align,
                                   Etox_Callback callback,
                                   Etox_Color color,
                                   Etox_Font font,
                                   Etox_Style style)
{
  Etox_Object_String string;

  if (!str)
    return NULL;

  string = (Etox_Object_String) malloc(sizeof(struct _Etox_Object_String));
  string->str = strdup(str);
  string->align = align;
  string->callback = callback;
  string->color = color;
  string->font = font;
  string->style = style;

  return string;
}

void
_etox_object_string_free(Etox_Object_String string)
{
  if (!string)
    return;
  IF_FREE(string->str);
  FREE(string);
}

Etox_Object_Newline
_etox_object_newline_new(void)
{
  Etox_Object_Newline nl;

  nl = (Etox_Object_Newline) malloc(sizeof(struct _Etox_Object_Newline));

  return nl;
}

void
_etox_object_newline_free(Etox_Object_Newline nl)
{
  if (!nl)
    return;
  FREE(nl);
}

Etox_Object_Tab
_etox_object_tab_new(Etox_Align align,
                     Etox_Callback callback,
                     Etox_Font font)
{
  Etox_Object_Tab tab;

  tab = (Etox_Object_Tab) malloc(sizeof(struct _Etox_Object_Tab));
  tab->align = align;
  tab->callback = callback;
  tab->font = font;

  return tab;
}

void
_etox_object_tab_free(Etox_Object_Tab tab)
{
  if (!tab)
    return;
  FREE(tab);
}
