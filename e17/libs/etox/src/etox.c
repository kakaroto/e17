#include "Etox_private.h"
#include "Etox.h"

void 
etox_bit_update_geometry(Etox e, Etox_Bit abit) 
{
   double obj_x, obj_y, obj_w, obj_h, max_x, max_y, min_x, min_y, max_ascent, max_descent;
   int i;
   
   abit->x = abit->y = abit->w = abit->h = 0;
   min_x = min_y = max_x = max_y = 0;
   if (e && abit && abit->evas_list && abit->font_style && (abit->font_style->num_bits>0) && (abit->num_evas > 0))
     {
	evas_get_geometry(e->evas, abit->evas_list[(abit->num_evas - 1)], &obj_x, &obj_y, &obj_w, &obj_h);
	evas_text_get_max_ascent_descent(e->evas, abit->evas_list[(abit->num_evas - 1)], &max_ascent, &max_descent);	
	obj_h = max_ascent - max_descent;
	for (i=0; i<abit->font_style->num_bits; i++) {
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
etox_bit_set_face(Etox_Bit bit, char *font, int font_size, Etox_Style style)
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
etox_clean(Etox e)
{

   /*
    * time to clean up any older crufty bits that could have been left around
    * from this particular Etox's previous incarnation.  This should do
    * everything but free the etox.  It is safe to call this multiple times.
    * This is also called from etox_free() - it is not necessary to call it
    * and then the etox_free() function.
    */

   int i,j;

   for(i=0;i<e->num_bits;i++)
     {
	for(j=0;j<e->bit_list[i]->num_evas;j++)
	  {
	     evas_del_object(e->evas, e->bit_list[i]->evas_list[j]);
	  }
	if(e->bit_list[i]->evas_list)
	  free(e->bit_list[i]->evas_list);
	if(e->bit_list[i]->font_style)
	  etox_style_free(e->bit_list[i]->font_style);
	if(e->bit_list[i]->font)
	  free(e->bit_list[i]->font);
     }

   if(e->bit_list)
     free(e->bit_list);

   e->bit_list = NULL;
   e->num_bits = 0;

   if(e->text)
     free(e->text);
   
   return;
}

int 
search_tokens(const char* text, const char** needles, int needles_count, 
              char* *beg, char* *next)
{
   /* 
    * Browse througn text, looking for any of the needles.
    * Upon finding one - set copy to beg the string between
    * the beginnig of text, and the first occurence of one 
    * of the needles. Set next to point to the remainig 
    * text right after the first found needle
    */
   
   char *tmp=NULL;
   int token_length=0;
   int needle_num = -1;
   int i;

   if (*beg)
     free(*beg);
   if (text)
     {
	for (i=0; i < needles_count; i++)
	  {
	     char *sec;

	     sec = strstr(text,needles[i]);
	     if (sec && ((!tmp) || ((tmp) && (sec<tmp))))
	       {
		  tmp = sec;
		  token_length = strlen(needles[i]);
		  needle_num = i;
	       }
	  }
	if (tmp)
	  {
	     *beg = malloc(sizeof(char) * (tmp - text +1));
	     strncpy(*beg, text, (tmp - text));
	     (*beg)[(tmp-text)] = '\0';
	     *next = tmp + token_length;
	  }
	else
	  {
	     *beg = strdup(text);
	     *next = NULL;
	  }
     }
   else
     {
	*beg = NULL;
	*next = NULL;
     }
   return needle_num;
}

void 
create_bit_objects(Etox_Bit abit, Etox e, Etox_Color text_color)
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

	switch(abit->font_style->bits[i].type)
	  {
	   case ETOX_STYLE_TYPE_OUTLINE:
             if (cb = etox_color_get_bit(e->color, "ol"))
   	       evas_set_color(e->evas, o, cb->r, cb->g, cb->b,
                              (abit->font_style->bits[i].alpha *
                               e->alpha_mod * cb->a / 65025));
	     break;
	   case ETOX_STYLE_TYPE_SHADOW:
             if (cb = etox_color_get_bit(e->color, "sh"))
               evas_set_color(e->evas, o, cb->r, cb->g, cb->b,
                              (abit->font_style->bits[i].alpha *
                               e->alpha_mod * cb->a / 65025));
	     break;
	   default:
	   case ETOX_STYLE_TYPE_FOREGROUND:
             if (cb = etox_color_get_bit(e->color, "fg"))             
               evas_set_color(e->evas, o, cb->r, cb->g, cb->b,        
                              (abit->font_style->bits[i].alpha *
                               e->alpha_mod * cb->a / 65025));
	     break;
	  }

	if(!abit->evas_list)
	  {
	     abit->evas_list = malloc((sizeof(Evas_Object *) *
				       abit->num_evas) + 1);
	  }
	else
	  {
	     abit->evas_list = realloc(abit->evas_list,
				       (sizeof(Evas_Object *) * abit->num_evas)
				       + 1);
	  }

	abit->evas_list[i] = o;

	/*
	 * make sure that we're stacked appropriately
	 */

	if(i > 0)
	  {
	     evas_stack_above(e->evas,o,abit->evas_list[i-1]);
	  }
     }
   etox_bit_update_geometry(e, abit);

   if (e->clip)
     etox_set_clip(e, e->clip);
   if (e->visible)
     etox_show(e);
}

void 
update_bit_objects(Etox_Bit abit, Etox e)
{
   double obj_x, obj_y, obj_w, obj_h;
   int i;

   for (i=0;i<abit->font_style->num_bits;i++)
     evas_set_text(e->evas, abit->evas_list[i], abit->text);
   etox_bit_update_geometry(e, abit);
}

void 
etox_bit_move_relative(Etox e, Etox_Bit abit, double delta_x, double delta_y)
{
   int i;
   double obj_x, obj_y, obj_w, obj_h;
   abit->x += delta_x;
   abit->y += delta_y;
   
   for (i=0; i<abit->num_evas; i++)
     {
	evas_get_geometry(e->evas,abit->evas_list[i],&obj_x, &obj_y, &obj_w, &obj_h);
	obj_x += delta_x;
	obj_y += delta_y;
	evas_move(e->evas, abit->evas_list[i], obj_x, obj_y);
     }
}

double 
dump_line(Etox_Bit *abits, int bit_count, Etox e, char align, 
          char vertical_align, double beg_x, double cur_w)
{
   int i;
   double line_height, line_width, obj_x, obj_y, obj_w, obj_h, delta_x, delta_y, ascent, descent, max_ascent;

   /*
    * Copy Etox_Bits from the buffer to Etox,
    * and place them accoring to lineheight and
    * current alignments
    */

   line_height = 0;
   line_width = 0;
   max_ascent = 0;
   /* Get line width and height */
   for (i=0; i<bit_count; i++) {
      if (abits[i]->h > line_height)
	line_height = abits[i]->h;
      if (i == (bit_count-1))
	line_width = abits[i]->x + abits[i]->w;      
      evas_text_get_max_ascent_descent(e->evas, abits[i]->evas_list[0], &ascent, &descent);
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
   for (i=0; i<bit_count; i++) {

      if (vertical_align == ETOX_ALIGN_TOP) 
	delta_y = 0;
      else if (vertical_align == ETOX_ALIGN_CENTER) 
	{
	   evas_text_get_max_ascent_descent(e->evas, abits[i]->evas_list[0], &ascent, &descent);	   
	   delta_y = (max_ascent - ascent) /2;
	}
      else if (vertical_align == ETOX_ALIGN_BOTTOM)
	{
	   evas_text_get_max_ascent_descent(e->evas, abits[i]->evas_list[0], &ascent, &descent);	   
	   delta_y = max_ascent - ascent;
	}
      /* Align the bit */
      etox_bit_move_relative(e, abits[i], delta_x, delta_y);

      /* Move it to the right position in the evas */
      etox_bit_move_relative(e, abits[i], e->x, e->y);
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
	  {
	     e->bit_list[e->num_bits - bit_count + i] = abits[i];
	  }
     }
   return line_height;
}

void 
find_available_size(Etox e, double beg_x, double beg_y, double h, 
                    double padding, double *av_x, double *av_y, double *av_w)
{
   /* 
    * Find the next available rectangle of the given height 
    * that is not clipped off
    */
   
   double x1, x2, y1, y2;
   int i, j;
   char ok = 0;
   
   x1 = beg_x;
   x2 = e->w;
   y1 = beg_y;
   y2 = beg_y + h;
   
   if (beg_x > e->w) {
      /* It's the endof the line - begin a new one */
      
      x1 = 0;
      x2 = e->w;
      y1 = beg_y + h + padding;
      y2 = y2 + h;
   }
   while (!ok) {
      ok = 1;
      for(i=0; i<e->num_rects; i++)
	{
	   if ((y1 <= (e->rect_list[i]->y) + (e->rect_list[i]->h)) &&
	       ((y2) >= e->rect_list[i]->y ))
	     {
		if ((x1 >= e->rect_list[i]->x) &&
		    (x1 <= (e->rect_list[i]->x + e->rect_list[i]->w)))
		  {
		     /* The beginning is inside a rectangle - move it out of it */  
		
		     x1 = e->rect_list[i]->x + e->rect_list[i]->w;
		     if (x1 >= x2) 
		       {
			  /* No chance of a new line on this height - move down */
			  ok = 0;
			  x1 = 0;
			  x2 = e->w;
			  y1 = (beg_y + h + padding);
			  y2 = y1 + h;
		       }
		  }
		if ((x2 >= e->rect_list[i]->x) &&
		    (x1 <= (e->rect_list[i]->x)))
		  {
		     /* 
		      * The rectangle is inbetween the beginnig and
		      * the end of the line - shorten the line
		      */
		     x2 = e->rect_list[i]->x;
		     if (x2 <= x1)
		       {
			  /* No chance of a new line on this height - move down */
			  ok = 0;
			  x1 = 0;
			  x2 = e->w;
			  y1 = (beg_y + h + padding);
			  y2 = y1 + h;
			  
		       }
		  }
	     }
	}
   }
   *av_x = x1;
   *av_y = y1;
   *av_w = x2 - x1;
}

void 
etox_show(Etox e)
{
   /* calls evas_show on all active Evas components */

   int i,j;

   if(!e)
     return;

   for (i = 0; i < e->num_bits; i++)
       for (j = 0; j < e->bit_list[i]->num_evas; j++)
	     evas_show(e->evas, e->bit_list[i]->evas_list[j]);
   e->visible = 1;
}

void
etox_hide(Etox e)
{
   /* calls evas_hide on all active Evas components */

   int i,j;

   if(!e)
     return;

   for (i = 0; i < e->num_bits; i++)
       for (j = 0; j < e->bit_list[i]->num_evas; j++)
          evas_hide(e->evas, e->bit_list[i]->evas_list[j]);
   e->visible = 0;
}

void 
etox_move(Etox e, double x, double y)
{
   /* this will move all the evas components in the etox */

   double delta_x, delta_y;
   int i,j;

   if(!e)
     return;

   delta_x = x - e->x;
   delta_y = y - e->y;

   e->x = x;
   e->y = y;

   /*
    * now we need to rotate through the list of evas inside of each bit and
    * move each of the evas components
    */

   for(i = 0; i < e->num_bits; i++)
     {
	e->bit_list[i]->x += delta_x;
	e->bit_list[i]->y += delta_y;
	for(j=0;j<e->bit_list[i]->num_evas;j++)
	  {
	     double old_y, old_x, w, h;

	     evas_get_geometry(e->evas,e->bit_list[i]->evas_list[j],&old_x,
			       &old_y,&w,&h);
	     evas_move(e->evas,e->bit_list[i]->evas_list[j],old_x + delta_x,
		       old_y + delta_y);
	  }
     }

}

void 
etox_resize(Etox e, double w, double h)
{

   /*
    * this will resize the container that the etox components are rendered
    * in.  Should force an etox_refresh
    */

   if(!e)
     return;

   e->w = w;
   e->h = h;

   etox_refresh(e);
}

void 
etox_refresh(Etox e)
{

   /*
    * This is a bit kludgy, but will work for now, since it forces a complete
    * rebuilding of the setup inside of the Etox object
    */

   if (e->text)
     {
	char *tmp = NULL;

	tmp = malloc(e->text_len + 1);
	if(tmp)
	  strcpy(tmp,e->text);
	etox_set_text(e,tmp);
	if(tmp)
	  free(tmp);
     }

}

void 
etox_free(Etox e)
{

   /*
    * this function is pretty straight forward.  most of the internals have
    * been moved into etox_clean, since it is used again in other places to
    * reset internal bits
    */

   int i;
   
   etox_clean(e);
   if (e->rect_list)
     {
	for (i=0; i<e->num_rects; i++)
	  free(e->rect_list[i]);
	free(e->rect_list);
     }
   if (e->font)
     free(e->font);
   if (e->color)
     etox_color_free(e->color);
   if (e->font_style)
     etox_style_free(e->font_style);
   free(e);

   return;
}

Etox_Bit
etox_bit_new()
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

Etox 
etox_new(Evas evas, char *name)
{
   /* initialize new Etox */

   Etox e;

   e = malloc(sizeof(struct _Etox));
   e->evas = evas;
   if (name)
     e->name = strdup(name);
   else
     e->name = NULL;
   e->text = NULL;
   e->text_len = 0;

   e->rect_list = NULL;
   e->num_rects = 0;
   e->rendered = 0;
   e->visible = 0;
   e->font=NULL;
   e->align = ETOX_ALIGN_LEFT;
   e->vertical_align = ETOX_ALIGN_BOTTOM;
   e->x = 0;
   e->y = 0;
   e->w = 0;
   e->h = 0;
   e->layer = 0;
   e->alpha_mod = 255;
   e->font_style = malloc(sizeof(struct _Etox_Style));
   e->font_style->name = strdup("default");
   e->font_style->bits = malloc(sizeof(struct _Etox_Style_Bit));
   e->font_style->bits[0].x = 0.0;
   e->font_style->bits[0].y = 0.0;
   e->font_style->bits[0].alpha = 255;
   e->font_style->bits[0].type = ETOX_STYLE_TYPE_FOREGROUND;
   e->font_style->num_bits = 1;
   e->font_style->in_use++;   
   e->bit_list = NULL;
   e->num_bits = 0; 
   e->font_size = 10;
   e->padding = 0;
   e->color = etox_color_new();
   etox_color_set_member(e->color, "fg", 255, 255, 255, 255);
   etox_color_set_member(e->color, "ol", 0, 0, 0, 255);
   etox_color_set_member(e->color, "sh", 0, 0, 0, 255);
   e->clip = NULL;

   return e;
}

Etox 
etox_new_all(Evas evas, char *name,
             double x, double y, double w, double h,
             char *font, int font_size, char *style_path,
             Etox_Color color, Etox_Align h_align, Etox_Align v_align,
             int layer, double padding)
{
   Etox e;

   e = malloc(sizeof(struct _Etox));
   
   if (name)
     e->name = strdup(name);
   else
     e->name = NULL;
   e->text = NULL;
   e->text_len = 0;   
   e->rect_list = NULL;
   e->num_rects = 0;
   e->rendered = 0;
   e->visible = 0;
   e->x = x;
   e->y = y;
   e->w = w;
   e->h = h;
   e->layer = layer;
   if (font)
     e->font = strdup(font);
   else
     e->font = NULL;
   if (style_path)
     e->font_style = etox_style_new(style_path);
   else
     {
	e->font_style = malloc(sizeof(struct _Etox_Style));

	e->font_style->name = strdup("default");
	e->font_style->bits = malloc(sizeof(struct _Etox_Style_Bit));
	
	e->font_style->bits[0].x = 0.0;
	e->font_style->bits[0].y = 0.0;
	e->font_style->bits[0].alpha = 255;
	e->font_style->bits[0].type = ETOX_STYLE_TYPE_FOREGROUND;
	e->font_style->num_bits = 1;
	e->font_style->in_use++;
     }
   e->bit_list = NULL;
   e->num_bits = 0;
   e->evas = evas;
   e->font_size = font_size;
   e->padding = padding;
   e->align = h_align;

   if (color)
     e->color = color;
   else
     {
       e->color = etox_color_new();
       etox_color_set_member(e->color, "fg", 255, 255, 255, 255);
       etox_color_set_member(e->color, "ol", 0, 0, 0, 255);
       etox_color_set_member(e->color, "sh", 0, 0, 0, 255);
     }
   e->vertical_align = v_align;
   e->alpha_mod = 255;
   e->clip = NULL;

   return e;
}

void
etox_raise(Etox e)
{
   int i, j;

   if (!e) return;

   for (i = 0; i < e->num_bits; i++)
      for (j = 0; j < e->bit_list[i]->num_evas; j++)
         evas_raise(e->evas, e->bit_list[i]->evas_list[j]);
}

void
etox_lower(Etox e)
{
   int i, j;

   if (!e) return;

   for (i = 0; i < e->num_bits; i++)
      for (j = 0; j < e->bit_list[i]->num_evas; j++)
         evas_lower(e->evas, e->bit_list[i]->evas_list[j]);
}
