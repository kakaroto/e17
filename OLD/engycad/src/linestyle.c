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

typedef struct _Line_Style Line_Style;

struct _Line_Style
{
    char               *name;
    int                 tiled;
    float               tilelength;
    int                 odd;
    int                 count;
    Eina_List          *points;
};

/* vars */
Eina_List          *line_styles = NULL;
Line_Style         *ls_current = NULL;

/* protos */
Line_Style         *_ls_load(const char *);
Line_Style         *_ls_find(const char *);

void
linestyle_set(const char *style)
{
    ls_current = _ls_find(style);
}

Line_Style         *
_ls_find(const char *style)
{
    Line_Style         *ls;

    Eina_List          *l;

    for (l = line_styles; l; l = l->next)
      {
          ls = (Line_Style *) l->data;
          if (!strcmp(style, ls->name))
              return ls;
      }
    ls = _ls_load(style);

    if (!ls)
        return NULL;

    line_styles = eina_list_append(line_styles, ls);
    return ls;
}

Line_Style         *
_ls_load(const char *style)
{
    Line_Style         *ls;
    Eina_List          *list = NULL;
    int                 i, res, tiled;
    char                buf[4096];
    float               x, y;

    XY                 *xy;

    sprintf(buf, "/linestyle/%s/tiled", style);
    E_DB_INT_GET(shell->line_styles_file, buf, tiled, res);
    if (!res)
        return NULL;

    ls = (Line_Style *) malloc(sizeof(Line_Style));
    ENGY_ASSERT(ls);
    memset(ls, 0, sizeof(Line_Style));
    ls->tiled = tiled;
    ls->name = DUP(style);

    sprintf(buf, "/linestyle/%s/tilelength", style);
    E_DB_FLOAT_GET(shell->line_styles_file, buf, ls->tilelength, res);
    ENGY_ASSERT(res);

    sprintf(buf, "/linestyle/%s/count", style);
    E_DB_INT_GET(shell->line_styles_file, buf, ls->count, res);
    ENGY_ASSERT(res);

    sprintf(buf, "/linestyle/%s/odd", style);
    E_DB_INT_GET(shell->line_styles_file, buf, ls->odd, res);
    ENGY_ASSERT(res);

    for (i = 0; i < ls->count; i++)
      {
          xy = (XY *) malloc(sizeof(XY));
          ENGY_ASSERT(xy);

          sprintf(buf, "/linestyle/%s/%d/h", style, i);
          E_DB_FLOAT_GET(shell->line_styles_file, buf, x, res);
          ENGY_ASSERT(res);
          xy->x = x;
          sprintf(buf, "/linestyle/%s/%d/v", style, i);
          E_DB_FLOAT_GET(shell->line_styles_file, buf, y, res);
          ENGY_ASSERT(res);
          xy->y = y;
          ls->points = eina_list_append(ls->points, xy);
      }

    return ls;
}

int
linestyle_get_tiled(void)
{
    if (!ls_current)
        return 0;
    return ls_current->tiled;
}

int
linestyle_get_odd(void)
{
    if (!ls_current)
        return 0;
    return ls_current->odd;
}

double
linestyle_get_tilelength(void)
{
    if (!ls_current)
        return 0;
    return (double)ls_current->tilelength;
}

Eina_List *
linestyle_get_points(double len, double scale)
{
    Line_Style         *ls;
    Eina_List          *res = NULL;
    Eina_List          *l;
    XY                 *a, *b;

    if (!ls_current)
        return NULL;

    ls = ls_current;

    if (ls->tiled)
      {
          double              tlen, dd, tscale;
          int                 i, n;

          tlen = ls->tilelength * scale;
          tscale = len / tlen;
          n = (int)(tscale + 2);

          dd = (n * tlen - len) / 2;

          a = (XY *) malloc(sizeof(XY));
          ENGY_ASSERT(a);
          a->x = 0.0;
          a->y = 0.0;
          res = eina_list_append(res, a);

          for (i = 0; i < n; i++)
            {
                for (l = ls->points; l; l = l->next)
                  {
                      a = (XY *) l->data;
                      b = (XY *) malloc(sizeof(XY));
                      ENGY_ASSERT(b);

                      b->x = a->x * scale - dd;
                      b->x += tlen * i;

                      if (b->x < 0)
                          b->x = 0;
                      if (b->x > len)
                          b->x = len;

                      b->y = a->y * scale;
                      res = eina_list_append(res, b);
                  }
            }
      }
    else
      {

          a = (XY *) malloc(sizeof(XY));
          ENGY_ASSERT(a);
          a->x = 0.0;
          a->y = 0.0;
          res = eina_list_append(res, a);

          for (l = ls->points; l; l = l->next)
            {
                b = (XY *) malloc(sizeof(XY));
                ENGY_ASSERT(b);
                memcpy(b, l->data, sizeof(XY));
                res = eina_list_append(res, b);
            }
      }
    return res;
}

Eina_List *
linestyle_get_dx_points(double x1, double x2, double scale)
{
    Line_Style         *ls;
    Eina_List          *res = NULL;
    Eina_List          *l;
    XY                 *a, *b;
    double              len;

    if (!ls_current)
        return NULL;

    ls = ls_current;

    if (x1 > x2)
        return 0;

    len = x2 - x1;

    if (ls->tiled)
      {
          double              tlen, dd, tscale;
          int                 i, n;

          tlen = ls->tilelength * scale;
          tscale = len / tlen;
          n = (int)(tscale + 2);

          dd = x1 - ((int)(x1 / tlen)) * tlen;

          a = (XY *) malloc(sizeof(XY));
          ENGY_ASSERT(a);
          a->x = x1;
          a->y = 0.0;
          res = eina_list_append(res, a);

          for (i = -1; i < n; i++)
            {
                for (l = ls->points; l; l = l->next)
                  {
                      a = (XY *) l->data;
                      b = (XY *) malloc(sizeof(XY));
                      ENGY_ASSERT(b);

                      b->x = a->x * scale;
                      b->x += tlen * i;

                      if (b->x < dd)
                          b->x = dd;
                      if (b->x > len + dd)
                          b->x = len + dd;

                      b->x += (float)((int)(x1 / tlen)) * tlen;
                      b->y = a->y * scale;
                      res = eina_list_append(res, b);
                  }
            }
      }
    else
      {
          double              tlen, tscale;

          a = (XY *) malloc(sizeof(XY));
          ENGY_ASSERT(a);
          a->x = x1;
          a->y = 0.0;
          res = eina_list_append(res, a);

          tlen = ls->tilelength;
          tscale = len / tlen;

          for (l = ls->points; l; l = l->next)
            {
                b = (XY *) malloc(sizeof(XY));
                ENGY_ASSERT(b);
                memcpy(b, l->data, sizeof(XY));
                b->x *= tscale;
                b->x += x1;
                res = eina_list_append(res, b);
            }
      }
    return res;
}
