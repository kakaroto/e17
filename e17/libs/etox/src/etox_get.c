#include "Etox_private.h"
#include "Etox.h"

void 
_etox_get_string_width(Etox e, Etox_Font font, char *str, double *w)
{
  static Evas_Object text = NULL;

  if (!e || !font || !str || !w)
    return;

  if (!text)
    text = evas_add_text(e->evas, font->name, font->size, str);
  else
    {
      evas_set_text(e->evas, text, str);
      evas_set_font(e->evas, text, font->name, font->size);
    }                                                    

  evas_get_geometry(e->evas, text, NULL, NULL, w, NULL);
}

void 
_etox_get_font_ascent_descent(Etox e, Etox_Font font,
                              double *ascent, double *descent)
{
  static Evas_Object text = NULL;

  if (!e || !font)
    return;

  if (!text)
    text = evas_add_text(e->evas, font->name, font->size, "");
  else
    evas_set_font(e->evas, text, font->name, font->size);

  evas_text_get_max_ascent_descent(e->evas, text, ascent, descent);
}

void
_etox_get_style_offsets(Etox_Style style, double *offset_w, double *offset_h)
{
  double max_x = 0.0, min_x = 0.0, max_y = 0.0, min_y = 0.0;
  Etox_Style_Bit style_bit;

  if (!style)
    return;

  /* get the width's offset caused by the style.. */
  ewd_list_goto_first(style->bits);
  while ((style_bit = (Etox_Style_Bit) ewd_list_next(style->bits)))
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
 
  *offset_w = max_x - min_x;
  *offset_h = max_y - min_y;
}


Evas            
etox_get_evas(Etox e)
{
  if (!e)
    return NULL;

  return e->evas;
}

char *          
etox_get_name(Etox e)
{
  if (!e)
    return NULL;

  return e->name;
}

int             
etox_get_alpha(Etox e)
{
  if (!e)
    return 0;

  return e->a;
}

double          
etox_get_padding(Etox e)
{
  if (!e)
    return 0.0;

  return e->padding;
}

int             
etox_get_layer(Etox e)
{
  if (!e)
    return 0;

  return e->layer;
}

Evas_Object     
etox_get_clip(Etox e)
{
  if (!e)
    return NULL;

  return e->clip;
}

Etox_Align_Type      
etox_get_align_v(Etox e)
{
  if (!e || !e->def.align)
    return ETOX_ALIGN_TYPE_NULL;

  return e->def.align->v;
}

Etox_Align_Type
etox_get_align_h(Etox e)
{
  if (!e || !e->def.align)
    return ETOX_ALIGN_TYPE_NULL;

  return e->def.align->h;
}

Etox_Color      
etox_get_color(Etox e)
{
  if (!e)
    return NULL;

  return e->def.color;
}

char *          
etox_get_font_name(Etox e)
{
  if (!e || !e->def.font)
    return NULL;

  return e->def.font->name;
}

int             
etox_get_font_size(Etox e)
{
  if (!e || e->def.font)
    return 0;

  return e->def.font->size;
}

Etox_Style      
etox_get_style(Etox e)
{
  if (!e)
    return NULL;

  return e->def.style;
}

char *          
etox_get_text(Etox e)
{
  Etox_Bit bit;
  Etox_Text text;
  char *p;
  int size = 0;

  if (!e || !e->bits || ewd_list_is_empty(e->bits))
    return NULL;

  ewd_list_goto_first(e->bits);
  while ((bit = (Etox_Bit) ewd_list_next(e->bits)))
    if (bit->type == ETOX_BIT_TYPE_TEXT)
      {
        text = (Etox_Text) bit->body;
        size += strlen(text->str);
      }

  p = (char *) malloc((sizeof(char) * size) + 1);

  ewd_list_goto_first(e->bits);
  while ((bit = (Etox_Bit) ewd_list_next(e->bits)))
    if (bit->type == ETOX_BIT_TYPE_TEXT)              
      {
        text = (Etox_Text) bit->body;
        strcat(p, text->str);  
      }

  return p;
}

int
etox_get_text_len(Etox e)
{
 	Etox_Bit bit;
	Etox_Text text;
	int size = 0;

	if (!e || !e->bits || ewd_list_is_empty(e->bits))
		return -1;

	ewd_list_goto_first(e->bits);
	while ((bit = ewd_list_next(e->bits)) != NULL)
		if (bit->type == ETOX_BIT_TYPE_TEXT)
		  {
			text = (Etox_Text) bit->body;
			size += strlen(text->str);
		  }

	return size;
}
void            
etox_get_geometry(Etox e, double *x, double *y, double *w, double *h)
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
etox_get_actual_geometry(Etox e, double *x, double *y, double *w, double *h)
{
  Etox_Object obj;
  double my_y, real_w, real_h;

  if (!e)
    return;

  if (x)
    *x = 0.0;
  if (y)
    *y = 0.0;
  if (w)
    *w = 0.0;
  if (h)
    *h = 0.0;

  if (!e->etox_objects || ewd_dlist_is_empty(e->etox_objects))
    return;

  ewd_dlist_goto_first(e->etox_objects);
  while ((obj = (Etox_Object) ewd_dlist_next(e->etox_objects)))
    {
      _etox_get_string_width(e, obj->bit.font, obj->str, &real_w);
      real_w += obj->bit.style->offset_w;
      if (real_w > *w)
        {
	  if (w)
            *w = real_w;
          switch (obj->bit.align->h)
            {
            case ETOX_ALIGN_TYPE_LEFT:
	      if (x)
                *x = obj->x;             
              break;     
            case ETOX_ALIGN_TYPE_CENTER:
            default:
              if (x)
	        *x = obj->x + ((obj->w - real_w) / 2);             
              break;                              
            case ETOX_ALIGN_TYPE_RIGHT:
              if (x)
	        *x = obj->x + obj->w - real_w;
              break; 
            }
        }
    }

  ewd_dlist_goto_first(e->etox_objects);
  obj = (Etox_Object) e->etox_objects->current->data;  
  real_h = (obj->bit.font->ascent - obj->bit.font->descent)
           + obj->bit.style->offset_h;
  switch (obj->bit.align->v)
    {
    case ETOX_ALIGN_TYPE_TOP:
      if (y)
        *y = obj->y;
      break;
    case ETOX_ALIGN_TYPE_CENTER:
    default:
      if (y)
        *y = obj->y + ((obj->h - real_h) / 2);
      break;                             
    case ETOX_ALIGN_TYPE_BOTTOM:   
      if (y)
        *y = obj->y + obj->h - real_h;
      break;
   }

  ewd_dlist_goto_last(e->etox_objects);
  obj = (Etox_Object) e->etox_objects->current->data;
  real_h = (obj->bit.font->ascent - obj->bit.font->descent)
           + obj->bit.style->offset_h;
  switch (obj->bit.align->v)
    {
    case ETOX_ALIGN_TYPE_TOP:
      my_y = obj->y;
      break;
    case ETOX_ALIGN_TYPE_CENTER:
    default:
      my_y = obj->y + ((obj->h - real_h) / 2);
      break;                             
    case ETOX_ALIGN_TYPE_BOTTOM:   
      my_y = obj->y + obj->h - real_h;
      break;                       
    }
  my_y += real_h;

  if (h)
    *h = my_y - *y;

  if(x)
    *x = ET_X_TO_EV(*x);
  if (y)
    *y = ET_Y_TO_EV(*y);
}

void            
etox_get_at(Etox e, int index, double *x, double *y, double *w, double *h)
{
  Evas_Object * obj;

  if (!e)
    return;

  if (x) *x = 0;
  if (y) *y = 0;
  if (w) *w = 0;
  if (h) *h = 0;

  ewd_dlist_goto_first(e->evas_objects);

  while ((obj = ewd_dlist_next(e->evas_objects)) != NULL)
    {
	evas_text_at(e->evas, obj, index, x, y, w, h);
	if (x || y || w || h)
	  break;
    }
}

int             
etox_get_at_position(Etox e, double x, double y,
                     double *char_x, double *char_y,
                     double *char_w, double *char_h)
{
  Evas_Object * to;
  int ret = 0;

  if (char_x)
  *char_x = 0.0;
  if (char_y)
    *char_y = 0.0;
  if (char_w)
    *char_w = 0.0;
  if (char_h)
    *char_h = 0.0;

  if (!e)
  	return ret;

  if (!e->etox_objects || ewd_dlist_is_empty(e->etox_objects))
    return ret;

  ewd_dlist_goto_first(e->evas_objects);

  while ((to = ewd_dlist_next(e->evas_objects)) != NULL)
    {
      double xx, yy, ww, hh;

      evas_get_geometry(e->evas, to, &xx, &yy, &ww, &hh);

      if (x < xx || y < yy ||
          x > xx + ww || y > yy + hh)
        continue;

        ret = evas_text_at_position(e->evas, to, x, y,
			char_x, char_y, char_w, char_h);

	break;
    }

  return ret;
}

int
etox_get_index_at(Etox e, double x, double y, int * i)
{
	Evas_Object * to;

	if (!e || !i || !e->evas_objects || ewd_list_is_empty(e->evas_objects))
		return -1;

	*i = 0;

	ewd_dlist_goto_first(e->evas_objects);

	while ((to = ewd_dlist_next(e->evas_objects)) != NULL)
	  {
		double xx, yy, ww, hh;

		evas_get_geometry(e->evas, to, &xx, &yy, &ww, &hh);

		if (x > xx && y > yy &&
		    x < xx + ww && y < yy + hh)
		  {
			double axx, ayy, aww, ahh;
			double bxx, byy, bww, bhh;
			int l = 0;
			int ii = -1;

			etox_get_at_position(e, x, y, &axx, &ayy, &aww, &ahh);

			l = etox_get_text_len(e);

			if (!l)
				return -1;

			while (ii++ < l)
			  {
				etox_get_at(e, ii, &bxx, &byy, &bww, &bhh);

				if (axx == bxx && ayy == byy &&
				    aww == bww && ahh == bhh)
				  {
					*i = ii;
					return 1;
				  }
			  }

			break;
		  }
	  }

	return -1;
}
