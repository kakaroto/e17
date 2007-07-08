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

typedef struct
{
    int                 num;
    char               *line_style;
    double              angle;
    double              xoffset;
    double              yoffset;
    double              ystep;
    double              scale;
}
HS_Item;

typedef struct
{
    int                 num;
    char               *name;
    Evas_List          *items;
}
HS;

/* protos */
HS                 *_hs_find(const char *);
HS                 *_hs_load(const char *);
HS_Item            *hs_load_item(const char *, int);

/* vars */
Evas_List          *hatches = NULL;
HS                 *hs_current = NULL;

void
hs_set(const char *style)
{
    hs_current = _hs_find(style);
}

HS                 *
_hs_find(const char *style)
{
    HS                 *hs;

    Evas_List          *l;

    for (l = hatches; l; l = l->next)
      {
          hs = (HS *) l->data;
          if (!strcmp(style, hs->name))
              return hs;
      }

    hs = _hs_load(style);

    if (!hs)
        return NULL;

    hatches = evas_list_append(hatches, hs);
    return hs;
}

HS                 *
_hs_load(const char *style)
{
    HS                 *hs;
    int                 i, num, res;
    char                buf[4096];
    float               x;

    sprintf(buf, "/hatchstyle/%s/num", style);
    E_DB_INT_GET(shell->hatch_styles_file, buf, i, res);
    ENGY_ASSERTS(res, "loading hatch style");
    if (!res)
        return NULL;

    hs = (HS *) malloc(sizeof(HS));
    ENGY_ASSERT(hs);
    memset(hs, 0, sizeof(HS));
    hs->num = i;
    hs->name = DUP(style);

    for (i = 0; i < hs->num; i++)
      {
          HS_Item            *it;

          it = hs_load_item(style, i);
          if (it)
              hs->items = evas_list_append(hs->items, it);
      }
    return hs;
}

HS_Item            *
hs_load_item(const char *style, int num)
{
    HS_Item            *it;
    char                buf[4096];
    char               *s;
    int                 i, res;
    float               x;

    if(strlen(style)>4000) return NULL;
    sprintf(buf, "/hatchstyle/%s/%d/linestyle", style, num);
    E_DB_STR_GET(shell->hatch_styles_file, buf, s, res);
    ENGY_ASSERT(res);
    if (!res)
        return NULL;
    if(s && (strlen(s)>4000))
	                s[4000]=0;

    it = (HS_Item *) malloc(sizeof(HS_Item));
    ENGY_ASSERT(it);
    memset(it, 0, sizeof(HS_Item));

    it->num = num;
    it->line_style = s;

    sprintf(buf, "/hatchstyle/%s/%d/angle", style, num);
    E_DB_FLOAT_GET(shell->hatch_styles_file, buf, x, res);
    ENGY_ASSERT(res);
    it->angle = x;

    sprintf(buf, "/hatchstyle/%s/%d/xoffset", style, num);
    E_DB_FLOAT_GET(shell->hatch_styles_file, buf, x, res);
    ENGY_ASSERT(res);
    it->xoffset = x;

    sprintf(buf, "/hatchstyle/%s/%d/yoffset", style, num);
    E_DB_FLOAT_GET(shell->hatch_styles_file, buf, x, res);
    ENGY_ASSERT(res);
    it->yoffset = x;

    sprintf(buf, "/hatchstyle/%s/%d/ystep", style, num);
    E_DB_FLOAT_GET(shell->hatch_styles_file, buf, x, res);
    ENGY_ASSERT(res);
    it->ystep = x;

    sprintf(buf, "/hatchstyle/%s/%d/scale", style, num);
    E_DB_FLOAT_GET(shell->hatch_styles_file, buf, x, res);
    ENGY_ASSERT(res);
    it->scale = x;

    return it;
}

int
hs_get_num(void)
{
    if (!hs_current)
        return 0;

    return hs_current->num;
}

double
hs_get_angle(int num)
{
    Evas_List          *l;
    HS_Item            *it;

    if (!hs_current)
        return 0.0;

    for (l = hs_current->items; l; l = l->next)
      {
          it = (HS_Item *) l->data;
          if (!it)
              continue;
          if (it->num == num)
              return it->angle;
      }
    return 0.0;
}

double
hs_get_xoffset(int num)
{
    Evas_List          *l;
    HS_Item            *it;

    if (!hs_current)
        return 0.0;

    for (l = hs_current->items; l; l = l->next)
      {
          it = (HS_Item *) l->data;
          if (!it)
              continue;
          if (it->num == num)
              return it->xoffset;
      }
    return 0.0;
}

double
hs_get_yoffset(int num)
{
    Evas_List          *l;
    HS_Item            *it;

    if (!hs_current)
        return 0.0;

    for (l = hs_current->items; l; l = l->next)
      {
          it = (HS_Item *) l->data;
          if (!it)
              continue;
          if (it->num == num)
              return it->yoffset;
      }
    return 0.0;
}

double
hs_get_ystep(int num)
{
    Evas_List          *l;
    HS_Item            *it;

    if (!hs_current)
        return 0.0;

    for (l = hs_current->items; l; l = l->next)
      {
          it = (HS_Item *) l->data;
          if (!it)
              continue;
          if (it->num == num)
              return it->ystep;
      }
    return 1.0;
}

double
hs_get_scale(int num)
{
    Evas_List          *l;
    HS_Item            *it;

    if (!hs_current)
        return 0.0;

    for (l = hs_current->items; l; l = l->next)
      {
          it = (HS_Item *) l->data;
          if (!it)
              continue;
          if (it->num == num)
              return it->scale;
      }
    return 1.0;
}

char               *
hs_get_line_style(int num)
{
    Evas_List          *l;
    HS_Item            *it;

    if (!hs_current)
        return NULL;

    for (l = hs_current->items; l; l = l->next)
      {
          it = (HS_Item *) l->data;
          if (!it)
              continue;
          if ((it->num == num) && (it->line_style))
              return DUP(it->line_style);
      }
    return NULL;
}
