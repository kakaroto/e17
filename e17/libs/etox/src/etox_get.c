#include "Etox_private.h"
#include "Etox.h"
#include <string.h>
#include <stdlib.h>

Evas
etox_get_evas(Etox e)
{
  if (!e) return NULL;
  return e->evas;
}

char *
etox_get_name(Etox e)
{
  if (!e) return NULL;
  return e->name;
}

Etox_Style
etox_get_style(Etox e)
{
  if (!e) return NULL;
  return e->font_style;
}

int
etox_get_layer(Etox e)
{
  if (!e) return 0;
  return e->layer;
}

double
etox_get_padding(Etox e)
{
  if (!e) return 0.0;
  return e->padding;
}

char *
etox_get_text(Etox e)
{
  if (!e) return NULL;
  return e->text;
}

Etox_Color
etox_get_color(Etox e)
{
  Etox_Color ret;
  if (!e) return NULL;
  ret = e->color;
  return ret;
}

int
etox_get_alpha(Etox e)
{
  if (!e) return 0;
  return e->alpha_mod;
}

void
etox_get_geometry(Etox e, double *x, double *y, double *w, double *h)
{
  if (!e) return;
  *x = e->x;
  *y = e->y;
  *w = e->w;
  *h = e->h;
}

void
etox_get_at(Etox e, int index, int *x, int *y, int *w, int *h)
{
  Etox_Bit bit;
  Evas_Object obj;
  char *p;
  int i, count;
  double dx = 0.0, dy = 0.0;

  if (!e || !e->num_bits)
    return; 

  count = 0;
  for (i = 0; i < e->num_bits; i++) 
    {
      if ((count + strlen(e->bit_list[i]->text)) >= index)
	{
	  bit = e->bit_list[i];
	  break;
	}
      count += strlen(e->bit_list[i]->text);
    }

  index -= count;

  count = 0;
  for (i = 0; i < bit->num_evas; i++)
    {
      p = evas_get_text_string(e->evas, bit->evas_list[i]);
      if ((count + strlen(p)) >= index)
	{
	  obj = bit->evas_list[i];
	  break;
	}
      count += strlen(p);
    }

  index -= count;

  evas_get_geometry(e->evas, obj, &dx, &dy, w, h);
  evas_text_at(e->evas, obj, index, x, y, w, h);

  *x = dx;
  *y = dy;
}

int
etox_get_at_position(Etox e, double x, double y,
		     int *char_x, int *char_y, int *char_w, int *char_h)
{
  Evas_Object obj = NULL;
  int i, j, ret;
  double my_x, my_y, my_w, my_h, dx = 0.0, dy = 0.0, size = 0.0;

  if (!e || !e->num_bits)
    return 0;

  for (i = 0; i < e->num_bits; i++)
    for (j = 0; j < e->bit_list[i]->num_evas; j++)
      {
	evas_get_geometry(e->evas, e->bit_list[i]->evas_list[j],
			  &my_x, &my_y, &my_w, &my_h);
	if ((my_x <= x) && ((my_x + my_w) >= x) &&
	    (my_y <= y) && ((my_y + my_h) >= y) )
          {
	    obj = e->bit_list[i]->evas_list[j];
	    break;
          }
      }

  if (!obj)
    return 0;
  evas_get_geometry(e->evas, obj, &dx, &dy, char_w, char_h);
  ret = evas_text_at_position(e->evas, obj, x, y,
			      char_x, char_y, char_w, char_h);

  *char_x = dx;
  *char_y = dy;

  return ret;
}

void
etox_get_size(Etox e, double *w, double *h)
{
  if (!e)
    return;
  *w = e->w;
  *h = e->h;
}

void
etox_get_actual_size(Etox e, double *w, double *h)
{
  int i, j;
  double my_x, my_y, my_w, my_h, prev_x, prev_y, prev_w;
  double tot_w, ret_w = 0.0, first_y = 0.0, last_y = 0.0;

  if (!e || !e->num_bits)
    return;

  for (i = 0; i < e->num_bits; i++)  
    { 
      prev_x = 0.0;
      prev_y = 0.0;
      tot_w = 0.0;
      for (j = 0; j < e->bit_list[i]->num_evas; j++)
	{
	  evas_get_geometry(e->evas, e->bit_list[i]->evas_list[j], 
			    &my_x, &my_y, &my_w, &my_h);
	  if (!prev_y || (prev_y == my_y))
	    tot_w += abs(my_x - prev_x);
	  else
	    {
	      tot_w += prev_w;
	      prev_x = my_x; 
	      prev_y = my_y; 
	      prev_w = my_w;
	      if (tot_w > ret_w)
		ret_w = tot_w;
	      tot_w = 0.0;
	      continue;
            }

          prev_x = my_x;
	  prev_y = my_y;
          prev_w = my_w;
	  if (tot_w > ret_w)
	    ret_w = tot_w;
	}
    }

  if (e->bit_list[0]->num_evas)
    evas_get_geometry(e->evas, e->bit_list[0]->evas_list[0],
		      &my_x, &first_y, &my_w, &my_h);
  if (e->bit_list[e->num_bits - 1]->num_evas)
    evas_get_geometry(e->evas, e->bit_list[e->num_bits - 1]->evas_list[0], 
                      &my_x, &last_y, &my_w, &my_h);

  *h = abs(last_y - first_y) + my_h;
  *w = ret_w;
}
