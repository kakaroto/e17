#include "Etox_private.h"
#include "Etox.h"

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
