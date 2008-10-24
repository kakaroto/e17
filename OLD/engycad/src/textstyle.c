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
    char               *font;
    int                 charmap;
    iconv_t             dcd;
    double              wscale;
    int                 bw;
    int                 ud;
    double              skew;
}
Text_Style;

/* vars */
Eina_List          *textstyles = NULL;
Text_Style         *ts_current = NULL;

/* protos */
Text_Style         *_ts_load(const char *);
Text_Style         *_ts_find(const char *);

void
textstyle_set(const char *style)
{
    ts_current = _ts_find(style);
}

Text_Style         *
_ts_find(const char *style)
{
    Text_Style         *ts;

    Eina_List          *l;

    for (l = textstyles; l; l = l->next)
      {
          ts = (Text_Style *) l->data;
          if (!strcmp(style, ts->name))
              return ts;
      }
    ts = _ts_load(style);

    if (!ts)
        return NULL;

    textstyles = eina_list_append(textstyles, ts);
    return ts;
}

Text_Style         *
_ts_load(const char *style)
{
    Text_Style         *ts;
    int                 i, res;
    char                buf[4096];
    float               x;
    char               *fr, *to;

    if(strlen(style)>4000) return NULL;

    sprintf(buf, "/textstyle/%s/charmap", style);
    E_DB_INT_GET(shell->text_styles_file, buf, i, res);
    ENGY_ASSERTS(res, "loading text style");
    if (!res)
        return NULL;

    ts = (Text_Style *) malloc(sizeof(Text_Style));
    ENGY_ASSERT(ts);
    memset(ts, 0, sizeof(Text_Style));

    ts->charmap = i;
    ts->name = DUP(style);

    sprintf(buf, "/textstyle/%s/fontname", style);
    E_DB_STR_GET(shell->text_styles_file, buf, ts->font, res);
    ENGY_ASSERT(res);
    if(ts->font && (strlen(ts->font)>4000))
	                ts->font[4000]=0;

    sprintf(buf, "/textstyle/%s/from", style);
    E_DB_STR_GET(shell->text_styles_file, buf, fr, res);
    ENGY_ASSERTS(res, "font encoding");

    sprintf(buf, "/textstyle/%s/to", style);
    E_DB_STR_GET(shell->text_styles_file, buf, to, res);
    ENGY_ASSERTS(res, "font encoding");

    if (!fr || !to)
      {
          FREE(ts);
          return NULL;
      }

    ts->dcd = iconv_open(to, fr);
    ENGY_ASSERTS(((long)ts->dcd != -1L), "iconv_open");
    FREE(fr);
    FREE(to);

    sprintf(buf, "/textstyle/%s/wscale", style);
    E_DB_FLOAT_GET(shell->text_styles_file, buf, x, res);
    ENGY_ASSERT(res);
    ts->wscale = x;

    sprintf(buf, "/textstyle/%s/bw", style);
    E_DB_INT_GET(shell->text_styles_file, buf, ts->bw, res);
    ENGY_ASSERT(res);

    sprintf(buf, "/textstyle/%s/ud", style);
    E_DB_INT_GET(shell->text_styles_file, buf, ts->ud, res);
    ENGY_ASSERT(res);

    sprintf(buf, "/textstyle/%s/skew", style);
    E_DB_FLOAT_GET(shell->text_styles_file, buf, x, res);
    ENGY_ASSERT(res);

    ts->skew = x;

    return ts;
}

char               *
textstyle_get_font(void)
{
    if (ts_current)
        return ts_current->font;
    return NULL;
}

int
textstyle_get_charmap(void)
{
    if (ts_current)
        return ts_current->charmap;
    return 0;
}

iconv_t
textstyle_get_dcd(void)
{
    if (ts_current)
        return ts_current->dcd;
    return 0;
}

double
textstyle_get_wscale(void)
{
    if (ts_current)
        return ts_current->wscale;
    return 1.0;
}

int
text_style_get_bw(void)
{
    if (ts_current)
        return ts_current->bw;
    return 0;
}

int
text_style_get_ud(void)
{
    if (ts_current)
        return ts_current->ud;
    return 0;
}

double
textstyle_get_skew(void)
{
    if (ts_current)
        return ts_current->skew;
    return 1.0;
}
