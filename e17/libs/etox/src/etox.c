#include "Etox_private.h"
#include "Etox.h"

int 
_etox_search_tokens(const char* text, const char** needles, int needles_count, 
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
_etox_available_size(Etox e, double beg_x, double beg_y, double h, 
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
   
  if (beg_x > e->w) 
    {
      /* It's the endof the line - begin a new one */
      
      x1 = 0;
      x2 = e->w;
      y1 = beg_y + h + padding;
      y2 = y2 + h;
    }
  while (!ok) 
    {
      ok = 1;
      for (i=0; i<e->num_rects; i++)
	{
	  if ((y1 <= (e->rect_list[i]->y) + (e->rect_list[i]->h)) &&
	      ((y2) >= e->rect_list[i]->y ))
	    {
	      if ((x1 >= e->rect_list[i]->x) &&
		  (x1 <= (e->rect_list[i]->x + e->rect_list[i]->w)))
		{
		  /* 
		   * The beginning is inside a rectangle - move it out of it
		   */  
		
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
_etox_clean(Etox e)
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
      for (j=0;j<e->bit_list[i]->num_evas;j++)
	evas_del_object(e->evas, e->bit_list[i]->evas_list[j]);
      if(e->bit_list[i]->evas_list)
	free(e->bit_list[i]->evas_list);
      if(e->bit_list[i]->font_style)
	etox_style_free(e->bit_list[i]->font_style);
      if(e->bit_list[i]->font)
	free(e->bit_list[i]->font);
    }

  if (e->bit_list)
    free(e->bit_list);

  e->bit_list = NULL;
  e->num_bits = 0;

  if (e->text)
    free(e->text);
   
  return;
}

void 
_etox_refresh(Etox e)
{
  /*
   * This is a bit kludgy, but will work for now, since it forces a complete
   * rebuilding of the setup inside of the Etox object
   */

  if (e->text)
    {
      char *tmp = NULL;

      tmp = malloc(e->text_len + 1);
      if (tmp)
	strcpy(tmp,e->text);
      etox_set_text(e,tmp);
      if (tmp)
	free(tmp);
    }
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
etox_free(Etox e)
{
  /*
   * this function is pretty straight forward.  most of the internals have
   * been moved into etox_clean, since it is used again in other places to
   * reset internal bits
   */

  int i;
   
  _etox_clean(e);
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

void 
etox_show(Etox e)
{
  /* calls evas_show on all active Evas components */

  int i,j;

  if (!e)
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

  if (!e)
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

  if (!e)
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

  if (!e)
    return;

  e->w = w;
  e->h = h;

  _etox_refresh(e);
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
