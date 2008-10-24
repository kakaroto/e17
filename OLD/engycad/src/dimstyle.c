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
    char               *name;
    char               *text_style;
    double              text_height;
    double              text_offset;

    char               *arrow_style;
    double              arrow_size;
    char               *leader_style;
    double              leader_size;

    char               *dline_style;
    double              dline_thickness;
    double              dline_scale;
    double              dline_prespacing;

    char               *eline_style;
    double              eline_thickness;
    double              eline_scale;
    double              eline_offset;
    double              eline_extend;
}
Dim_Style;

/** vars **/
Eina_List          *dim_styles = NULL;
Dim_Style          *ds_current = NULL;

/* protos */
Dim_Style          *ds_load(const char *);
Dim_Style          *ds_find(const char *);

void
dimstyle_set(const char *style)
{
    ds_current = ds_find(style);
}

Dim_Style          *
ds_find(const char *style)
{
    Dim_Style          *ds;
    Eina_List          *l;

    for (l = dim_styles; l; l = l->next)
      {
          ds = (Dim_Style *) l->data;
          if (!strcmp(ds->name, style))
              return ds;
      }

    ds = ds_load(style);

    if (!ds)
        return NULL;

    dim_styles = eina_list_append(dim_styles, ds);
    return ds;
}

Dim_Style          *
ds_load(const char *style)
{
    Dim_Style          *ds;
    int                 i, res;
    char                buf[4096];
    char               *s;
    float               x, y;

    if(!style) return NULL;
    
    if(strlen(style) > 4000) return NULL;

    sprintf(buf, "/dimstyle/%s/textstyle", style);
    E_DB_STR_GET(shell->dim_styles_file, buf, s, res);
    if (!res)
        return NULL;
    if(s && (strlen(s)>4000))s[4000]=0;

    ds = (Dim_Style *) malloc(sizeof(Dim_Style));
    ENGY_ASSERT(ds);
    memset(ds, 0, sizeof(Dim_Style));
    ds->name = DUP(style);
    ds->text_style = s;

    sprintf(buf, "/dimstyle/%s/textheight", style);
    E_DB_FLOAT_GET(shell->dim_styles_file, buf, x, res);
    ENGY_ASSERT(res);
    ds->text_height = x;

    sprintf(buf, "/dimstyle/%s/textoffset", style);
    E_DB_FLOAT_GET(shell->dim_styles_file, buf, x, res);
    ENGY_ASSERT(res);
    ds->text_offset = x;

    sprintf(buf, "/dimstyle/%s/arrowstyle", style);
    E_DB_STR_GET(shell->dim_styles_file, buf, s, res);
    ENGY_ASSERT(res);
    if(s && (strlen(s)>4000))s[4000]=0;
    ds->arrow_style = s;

    sprintf(buf, "/dimstyle/%s/arrowsize", style);
    E_DB_FLOAT_GET(shell->dim_styles_file, buf, x, res);
    ENGY_ASSERT(res);
    ds->arrow_size = x;

    sprintf(buf, "/dimstyle/%s/leaderstyle", style);
    E_DB_STR_GET(shell->dim_styles_file, buf, s, res);
    ENGY_ASSERT(res);
    if(s && (strlen(s)>4000))s[4000]=0;
    ds->leader_style = s;

    sprintf(buf, "/dimstyle/%s/leadersize", style);
    E_DB_FLOAT_GET(shell->dim_styles_file, buf, x, res);
    ENGY_ASSERT(res);
    ds->leader_size = x;

    sprintf(buf, "/dimstyle/%s/dlinestyle", style);
    E_DB_STR_GET(shell->dim_styles_file, buf, s, res);
    ENGY_ASSERT(res);
    if(s && (strlen(s)>4000))s[4000]=0;
    ds->dline_style = s;

    sprintf(buf, "/dimstyle/%s/dlinethickness", style);
    E_DB_FLOAT_GET(shell->dim_styles_file, buf, x, res);
    ENGY_ASSERT(res);
    ds->dline_thickness = x;

    sprintf(buf, "/dimstyle/%s/dlinescale", style);
    E_DB_FLOAT_GET(shell->dim_styles_file, buf, x, res);
    ENGY_ASSERT(res);
    ds->dline_scale = x;

    sprintf(buf, "/dimstyle/%s/dlineprespacing", style);
    E_DB_FLOAT_GET(shell->dim_styles_file, buf, x, res);
    ENGY_ASSERT(res);
    ds->dline_prespacing = x;

    sprintf(buf, "/dimstyle/%s/elinestyle", style);
    E_DB_STR_GET(shell->dim_styles_file, buf, s, res);
    ENGY_ASSERT(res);
    if(s && (strlen(s)>4000))s[4000]=0;
    ds->eline_style = s;

    sprintf(buf, "/dimstyle/%s/elinethickness", style);
    E_DB_FLOAT_GET(shell->dim_styles_file, buf, x, res);
    ENGY_ASSERT(res);
    ds->eline_thickness = x;

    sprintf(buf, "/dimstyle/%s/elinescale", style);
    E_DB_FLOAT_GET(shell->dim_styles_file, buf, x, res);
    ENGY_ASSERT(res);
    ds->eline_scale = x;

    sprintf(buf, "/dimstyle/%s/elineoffset", style);
    E_DB_FLOAT_GET(shell->dim_styles_file, buf, x, res);
    ENGY_ASSERT(res);
    ds->eline_offset = x;

    sprintf(buf, "/dimstyle/%s/elineextend", style);
    E_DB_FLOAT_GET(shell->dim_styles_file, buf, x, res);
    ENGY_ASSERT(res);
    ds->eline_extend = x;

    return ds;
}

char               *
dimstyle_get_text_style(void)
{
    if (!ds_current)
        return NULL;
    return DUP(ds_current->text_style);
}

double
dimstyle_get_text_height(void)
{
    if (!ds_current)
        return 0.0;
    return ds_current->text_height;
}

double
dimstyle_get_text_offset(void)
{
    if (!ds_current)
        return 0.0;
    return ds_current->text_offset;
}

char               *
dimstyle_get_arrow_style(void)
{
    if (!ds_current)
        return NULL;
    return DUP(ds_current->arrow_style);
}

double
dimstyle_get_arrow_size(void)
{
    if (!ds_current)
        return 1.0;
    return ds_current->arrow_size;
}

char               *
dimstyle_get_dim_line_style(void)
{
    if (!ds_current)
        return NULL;
    return DUP(ds_current->dline_style);
}

double
dimstyle_get_dim_line_thickness(void)
{
    if (!ds_current)
        return 0.0;
    return ds_current->dline_thickness;
}

double
dimstyle_get_dim_line_scale(void)
{
    if (!ds_current)
        return 1.0;
    return ds_current->dline_scale;
}

double
dimstyle_get_dim_line_prespacing(void)
{
    if (!ds_current)
        return 0.0;
    return ds_current->dline_prespacing;
}

char               *
dimstyle_get_ext_line_style(void)
{
    if (!ds_current)
        return NULL;
    return DUP(ds_current->eline_style);
}

double
dimstyle_get_ext_line_thickness(void)
{
    if (!ds_current)
        return 0.0;
    return ds_current->eline_thickness;
}

double
dimstyle_get_ext_line_scale(void)
{
    if (!ds_current)
        return 1.0;
    return ds_current->eline_scale;
}

double
dimstyle_get_ext_line_offset(void)
{
    if (!ds_current)
        return 0.0;
    return ds_current->eline_offset;
}

double
dimstyle_get_ext_line_extend(void)
{
    if (!ds_current)
        return 0.0;
    return ds_current->eline_extend;
}

char               *
dimstyle_get_leader_style(void)
{
    if (!ds_current)
        return NULL;
    return DUP(ds_current->leader_style);
}

double
dimstyle_get_leader_size(void)
{
    if (!ds_current)
        return 0.0;
    return ds_current->leader_size;
}
