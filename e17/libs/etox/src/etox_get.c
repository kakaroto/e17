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
  ret = &(e->color);
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

  evas_text_at(e->evas, obj, index, x, y, w, h);

  *x += abs(abs(e->x) - abs(bit->x));
  *y += abs(abs(e->y) - abs(bit->y));
}

int
etox_get_at_position(Etox e, int x, int y, int *index)
{
  /* sorry, laterz.. */

  return 0;
}
