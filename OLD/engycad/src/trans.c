/*
 * Copyright (C) 2002, Yury Hudobin
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies of the Software, its documentation and marketing & publicity
 * materials, and acknowledgment shall be given in the documentation, materials
 * and software packages that this Software was used.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "engy.h"

void
trans_move(Eina_List *list, double dx, double dy)
{
    Eina_List          *l;
    XY                 *o;

    for (l = list; l; l = l->next)
      {
          o = (XY *) l->data;
          o->x += dx;
          o->y += dy;
      }
}

void
trans_scale(Eina_List *list, double xscale, double yscale)
{
    Eina_List          *l;
    XY                 *o;

    for (l = list; l; l = l->next)
      {
          o = (XY *) l->data;
          o->x *= xscale;
          o->y *= yscale;
      }
}

void
trans_rotate(Eina_List *list, double angle)
{
    Eina_List          *l;
    XY                 *o;
    double              x;
    double              cos_a, sin_a;

    cos_a = cos(angle);
    sin_a = sin(angle);

    for (l = list; l; l = l->next)
      {
          o = (XY *) l->data;
          x = o->x;
          o->x = x * cos_a + o->y * sin_a;
          o->y = o->y * cos_a - x * sin_a;
      }
}

double
unit(int unit)
{
    switch (unit)
      {
      case UNIT_NONE:
          return 1e0;
      case UNIT_INCH:
          return 2.54e-2;
      case UNIT_FOOT:
          return 3.048e-1;
      case UNIT_MILE:
          return 1.6093e3;
      case UNIT_MILLIMETER:
          return 1e-3;
      case UNIT_SANTIMETER:
          return 1e-2;
      case UNIT_METER:
          return 1e0;
      case UNIT_KILOMETER:
          return 1e3;
      case UNIT_MICROINCH:
          return 2.54e-8;
      case UNIT_MIL:
          return 2.54e-5;
      case UNIT_YARD:
          return 9.144e-1;
      case UNIT_ANGSTROM:
          return 1e-10;
      case UNIT_NANOMETER:
          return 1e-9;
      case UNIT_MICRON:
          return 1e-6;
      case UNIT_DECIMETER:
          return 1e-1;
      case UNIT_DECAMETER:
          return 1e1;
      case UNIT_HECTOMETER:
          return 1e2;
      case UNIT_GIGAMETER:
          return 1e9;
      case UNIT_ASTRO:
          return 1.496e11;
      case UNIT_LIGHTYEAR:
          return 9.4605e15;
      case UNIT_PARSEC:
          return 3.0857e16;
      }
}
