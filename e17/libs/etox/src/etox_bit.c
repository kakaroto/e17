#include "Etox_private.h"
#include "Etox.h"

Etox_Bit
_etox_bit_new(void)
{
  /* initialiazation of new Etox_Bit data structures */

  Etox_Bit bit;

  bit = malloc(sizeof(struct _Etox_Bit));
  bit->text = NULL;
  bit->x = 0;
  bit->y = 0;
  bit->w = 0;
  bit->h = 0;
  bit->font = NULL;
  bit->evas_list = NULL;
  bit->num_evas = 0;
  bit->font_style = NULL;
  bit->font_size = 0;

  return bit;
}

void 
_etox_bit_update_geometry(Etox e, Etox_Bit abit) 
{
  double obj_x, obj_y, obj_w, obj_h, max_x, max_y, min_x, min_y;
  double max_ascent, max_descent;
  int i;
   
  abit->x = abit->y = abit->w = abit->h = 0;
  min_x = min_y = max_x = max_y = 0;
  if (e && abit && abit->evas_list && abit->font_style && 
      (abit->font_style->num_bits>0) && (abit->num_evas > 0))
    {
      evas_get_geometry(e->evas, abit->evas_list[(abit->num_evas - 1)], 
			&obj_x, &obj_y, &obj_w, &obj_h);
      evas_text_get_max_ascent_descent(e->evas, 
				       abit->evas_list[(abit->num_evas - 1)], 
				       &max_ascent, &max_descent);	
      obj_h = max_ascent - max_descent;
      for (i=0; i<abit->font_style->num_bits; i++) 
	{
	  if (min_x > abit->font_style->bits[i].x)
	    min_x = abit->font_style->bits[i].x;
	  else if (max_x < abit->font_style->bits[i].x)
	    max_x = abit->font_style->bits[i].x;
	  if (min_y > abit->font_style->bits[i].y)
	    min_y = abit->font_style->bits[i].y;
	  else if (max_y < abit->font_style->bits[i].y)
	    max_y = abit->font_style->bits[i].y;
	}
      abit->x = obj_x;
      abit->y = obj_y;
      abit->w = obj_w + (max_x - min_x);
      abit->h = obj_h + (max_y - min_y);
    }
}

void 
_etox_bit_set_face(Etox_Bit bit, char *font, int font_size, Etox_Style style)
{
  if(bit->font)
    free(bit->font);
  bit->font = malloc((strlen(font) * sizeof(char)) + 1);
  strcpy(bit->font,font);
  bit->font_size = font_size;
  bit->font_style = style;
  style->in_use++;
}

void 
_etox_bit_create_objects(Etox_Bit abit, Etox e, Etox_Color text_color)
{
  /* Create the evas objects than make up this Etox_Bit */
   
  double obj_x, obj_y, obj_w, obj_h;
  int i;

  for (i=0;i<abit->font_style->num_bits;i++)
    {
      Evas_Object *o;
      Etox_Color_Bit cb;

      o = evas_add_text(e->evas,abit->font,abit->font_size,
			abit->text);
      
      evas_move(e->evas,o,abit->x + abit->font_style->bits[i].x,
		abit->y + abit->font_style->bits[i].y);

      evas_set_layer(e->evas, o, e->layer);

      abit->num_evas++;

      switch (abit->font_style->bits[i].type)
	{
	case ETOX_STYLE_TYPE_OUTLINE:
	  if (cb = _etox_color_get_bit(e->color, "ol"))
	    evas_set_color(e->evas, o, cb->r, cb->g, cb->b,
			   (abit->font_style->bits[i].alpha *
			    e->alpha_mod * cb->a / 65025));
	  break;
	case ETOX_STYLE_TYPE_SHADOW:
	  if (cb = _etox_color_get_bit(e->color, "sh"))
	    evas_set_color(e->evas, o, cb->r, cb->g, cb->b,
			   (abit->font_style->bits[i].alpha *
			    e->alpha_mod * cb->a / 65025));
	  break;
	default:
	case ETOX_STYLE_TYPE_FOREGROUND:
	  if (cb = _etox_color_get_bit(e->color, "fg"))             
	    evas_set_color(e->evas, o, cb->r, cb->g, cb->b,        
			   (abit->font_style->bits[i].alpha *
			    e->alpha_mod * cb->a / 65025));
	  break;
	}
      
      if(!abit->evas_list)
	abit->evas_list = malloc((sizeof(Evas_Object *) *
				    abit->num_evas) + 1);
      else
	abit->evas_list = realloc(abit->evas_list,
				  (sizeof(Evas_Object *) * abit->num_evas)
				  + 1);

      abit->evas_list[i] = o;
      
      /*
       * make sure that we're stacked appropriately
       */

      if (i > 0)
	evas_stack_above(e->evas,o,abit->evas_list[i-1]);
    }
  _etox_bit_update_geometry(e, abit);

  if (e->clip)
    etox_set_clip(e, e->clip);
  if (e->visible)
    etox_show(e);
}

void 
_etox_bit_update_objects(Etox_Bit abit, Etox e)
{
  double obj_x, obj_y, obj_w, obj_h;
  int i;

  for (i=0;i<abit->font_style->num_bits;i++)
    evas_set_text(e->evas, abit->evas_list[i], abit->text);
  _etox_bit_update_geometry(e, abit);
}

void 
_etox_bit_move_relative(Etox e, Etox_Bit abit, double delta_x, double delta_y)
{
  int i;
  double obj_x, obj_y, obj_w, obj_h;
  abit->x += delta_x;
  abit->y += delta_y;
   
  for (i=0; i<abit->num_evas; i++)
    {
      evas_get_geometry(e->evas,abit->evas_list[i],&obj_x, 
			&obj_y, &obj_w, &obj_h);
      obj_x += delta_x;
      obj_y += delta_y;
      evas_move(e->evas, abit->evas_list[i], obj_x, obj_y);
    }
}

double 
_etox_bit_dump_line(Etox_Bit *abits, int bit_count, Etox e, char align, 
                    char vertical_align, double beg_x, double cur_w)
{
  int i;
  double line_height, line_width, obj_x, obj_y, obj_w, obj_h;
  double delta_x, delta_y, ascent, descent, max_ascent;

  /*
   * Copy Etox_Bits from the buffer to Etox,
   * and place them accoring to lineheight and
   * current alignments
   */

  line_height = 0;
  line_width = 0;
  max_ascent = 0;
  /* Get line width and height */
  for (i=0; i<bit_count; i++) 
    {
      if (abits[i]->h > line_height)
	line_height = abits[i]->h;
      if (i == (bit_count-1))
	line_width = abits[i]->x + abits[i]->w;      
      evas_text_get_max_ascent_descent(e->evas, abits[i]->evas_list[0], 
				       &ascent, &descent);
      if (ascent > max_ascent)
	max_ascent = ascent;
    }

  if (align == ETOX_ALIGN_LEFT)
    delta_x = 0;
  else if (align == ETOX_ALIGN_CENTER) 
    delta_x = (beg_x + cur_w - line_width) / 2;
  else if (align == ETOX_ALIGN_RIGHT)
    delta_x = beg_x + (cur_w - line_width);

  /* Move to properly aligned postion */
  for (i=0; i<bit_count; i++) 
    {
      if (vertical_align == ETOX_ALIGN_TOP) 
	delta_y = 0;
      else if (vertical_align == ETOX_ALIGN_CENTER) 
	{
	  evas_text_get_max_ascent_descent(e->evas, abits[i]->evas_list[0], 
					   &ascent, &descent);	   
	  delta_y = (max_ascent - ascent) /2;
	}
      else if (vertical_align == ETOX_ALIGN_BOTTOM)
	{
	  evas_text_get_max_ascent_descent(e->evas, abits[i]->evas_list[0], 
					   &ascent, &descent);	   
	  delta_y = max_ascent - ascent;
	}
      /* Align the bit */
      _etox_bit_move_relative(e, abits[i], delta_x, delta_y);

      /* Move it to the right position in the evas */
      _etox_bit_move_relative(e, abits[i], e->x, e->y);
    }
   
   
  if (bit_count)
    {
      if(e->bit_list)
	{
	  e->num_bits += bit_count;
	  e->bit_list = realloc(e->bit_list,
				sizeof(Etox_Bit) * (e->num_bits));
	}
      else
	{
	  e->num_bits += bit_count;
	  e->bit_list = malloc(sizeof(Etox_Bit) *
			       (e->num_bits));
	}

      for (i=0; i<bit_count; i++)
	e->bit_list[e->num_bits - bit_count + i] = abits[i];
    }
  return line_height;
}
