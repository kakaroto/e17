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
#include <math.h>

struct Log_Param
{
    Color               color;
    char               *font;
    int                 fontsize;
    int                 fontsize2;
    int                 charmap;
    double              text_height;
    iconv_t             dcd;
};

struct Log_Param    log_param;

Evas_Object        *info_bg;
Evas_Object        *log_text[10];
Evas_Object        *log_bg[10];
int                 step = 20;
int                 log_visible = 0;
int                 info_visible = 1;
Evas_Object        *info_text[20];

void
log_init(void)
{
    Evas               *e;
    int                 i, res;
    Evas_Coord          w, as, des;
    char               *s;

    e = shell->evas;

    E_DB_STR_GET(shell->rcfile, "/main_win/cl/font", s, res);
    ENGY_ASSERT(res);
    log_param.font = s;

    E_DB_INT_GET(shell->rcfile, "/main_win/cl/fontsize", i, res);
    ENGY_ASSERT(res);
    log_param.fontsize = i;

    E_DB_INT_GET(shell->rcfile, "/main_win/cl/fontsize2", i, res);
    ENGY_ASSERT(res);
    log_param.fontsize2 = i;

    E_DB_INT_GET(shell->rcfile, "/main_win/cl/charmap", i, res);
    ENGY_ASSERT(res);
    log_param.charmap = i;
    imlib_context_set_TTF_encoding(log_param.charmap);

    log_param.dcd = shell->dcd;

    for (i = 0; i < 10; i++)
      {
          Evas_Object        *o;
          char               *s;

          o = evas_object_rectangle_add(e);
          evas_object_layer_set(o, 17);
          evas_object_pass_events_set(o, 1);
          log_bg[i] = o;

          o = evas_object_text_add(e);
	  evas_object_text_font_set(o, log_param.font, log_param.fontsize);
	  evas_object_text_text_set(o, "");
          evas_object_layer_set(o, 17);
          evas_object_pass_events_set(o, 1);
          log_text[i] = o;
      }
    as = evas_object_text_max_ascent_get(log_text[0]);
    des = evas_object_text_max_descent_get(log_text[0]);
    log_param.text_height = as - des;
    log_show();
}

void
log_add_string(char *s)
{
    Evas               *e;
    int                 i;
    double              w, h;

    e = shell->evas;
    h = log_param.text_height + 2;

    if (s)
      {
          char               *s1;

          step++;
          s1 = my_iconv(shell->dcd, s);
	  evas_object_text_text_set(log_text[step % 10], s1);
          FREE(s);
          FREE(s1);
          w = evas_object_text_horiz_advance_get(log_text[step % 10]);
          evas_object_resize(log_bg[step % 10], w + 20, h);
      }

    for (i = 0; i < 10; i++)
      {
          int                 k;
	  char 		      a;

          k = (step - i) % 10;
	  a = 120 - 12 * i;
          evas_object_move(log_bg[k], 0, shell->h - h * (i + 1) - 18);
          evas_object_color_set(log_bg[k], 200*a/255, 200*a/255, a, a);
//      evas_object_show(log_bg[k]);
          evas_object_move(log_text[k], 10, shell->h - h * (i + 1) - 18);
          evas_object_color_set(log_text[k], 0, 0, 0, 250 - 25 * i);
//      evas_object_show(log_text[k]);
      }
}

void
log_configure(void)
{
    log_add_string(NULL);
}

static int
log_anim_show(void *data)
{
    long val = (long)data;
    if (val < 11)
      {
          evas_object_show(log_text[(step - val + 1) % 10]);
          evas_object_show(log_bg[(step - val + 1) % 10]);
          ecore_timer_add(0.05, log_anim_show, (void*)(val + 1));
      }
    return 0;
}

void
log_show(void)
{
    log_visible = 1;
    log_anim_show(NULL);
}

static int
log_anim_hide(void *data)
{
    long val = (long)data;
    if (val < 11)
      {
          evas_object_hide(log_text[(step - val + 1) % 10]);
          evas_object_hide(log_bg[(step - val + 1) % 10]);
          ecore_timer_add(0.05, log_anim_hide, (void*)(val + 1));
      }
    return 0;
}

void
log_hide(void)
{
    log_visible = 0;
    log_anim_hide(NULL);
}

void
log_toggle(void)
{
    if (log_visible)
      {
          log_hide();
      }
    else
      {
          log_show();
      };
}

/*******************************************************/

typedef struct
{
    Evas_Object        *sep0;
    Evas_Object        *color;
    Evas_Object        *lstyle;
    Evas_Object        *lscale;
    Evas_Object        *thickness;
    Evas_Object        *tstyle;
    Evas_Object        *gravity;
    Evas_Object        *pstyle;
/*    Evas_Object prec;   */
/*    Evas_Object snap;   */
/*    Evas_Object segnum; */
/*    Evas_Object arcnum; */
/*    Evas_Object f_prec; */
    Evas_Object        *sep1;
    Evas_Object        *obj_color;
    Evas_Object        *obj_properties[10];
    Evas_Object        *sep2;
    Evas_Object        *last_xy;
    Evas_Object        *curr_xy;
}
Info_Entries;

Info_Entries        info;
Evas_Object        *info_bg;
int                 max_width = 90;
int                 real_width;
float               width = 1.0;

void
info_init(void)
{
    Evas               *e;
    Evas_Object        *o;
    int                 i, n;
    Evas_Object       **p;
    char               *s;

    e = shell->evas;

    info_bg = evas_object_rectangle_add(e);
    evas_object_color_set(info_bg, 100, 100, 120, 120);
    evas_object_layer_set(info_bg, 17);
    evas_object_pass_events_set(info_bg, 1);
    evas_object_show(info_bg);

    n = sizeof(info) / sizeof(Evas_Object*);
    memset(&info, 0, sizeof(info));

    info.color = evas_object_rectangle_add(e);
    evas_object_color_set(info.color,
	shell->context.color.red*shell->context.color.alpha/255,
	shell->context.color.green*shell->context.color.alpha/255,
	shell->context.color.blue*shell->context.color.alpha/255, 
	shell->context.color.alpha);
    evas_object_layer_set(info.color, 17);
    evas_object_pass_events_set(info.color, 1);
    evas_object_resize(info.color, max_width - 16, log_param.fontsize2);
    evas_object_show(info.color);

    {
        Color               col = common_get_color();

        info.obj_color = evas_object_rectangle_add(e);
        evas_object_color_set(info.obj_color,
		col.red*col.alpha/255,
		col.green*col.alpha/255,
		col.blue*col.alpha/255,
		col.alpha);
        evas_object_layer_set(info.obj_color, 17);
        evas_object_pass_events_set(info.obj_color, 1);
        evas_object_resize(info.obj_color, max_width - 16, log_param.fontsize2);
        evas_object_show(info.obj_color);
    }

    p = (Evas_Object **) & info;
    for (i = 0; i < n; i++)
      {
          Evas_Object        **o;

          o = p + i;
          if (*o)
              continue;
          *o = evas_object_text_add(e);
	  evas_object_text_font_set(*o, log_param.font, log_param.fontsize2);
	  evas_object_text_text_set(*o, "");
          evas_object_color_set(*o, 0, 0, 0, 150);
          evas_object_layer_set(*o, 17);
          evas_object_pass_events_set(*o, 1);
          evas_object_show(*o);
      }

    width = 1;
    real_width = max_width * width;

    s = my_iconv(shell->dcd, _("-- Shell properties  --"));
    evas_object_text_text_set(info.sep0, s);
    IF_FREE(s);

    s = my_iconv(shell->dcd, _("-- Object properties --"));
    evas_object_text_text_set(info.sep1, s);
    IF_FREE(s);

    s = my_iconv(shell->dcd, _("-- Mouse coordinates --"));
    evas_object_text_text_set(info.sep2, s);
    IF_FREE(s);
}

void
info_configure(void)
{
    Evas               *e;
    int                 i, n;
    Evas_Object       **p;

    e = shell->evas;

    evas_object_resize(info_bg, real_width + 16, shell->h);
    evas_object_move(info_bg, shell->w - real_width - 16, 0);

    n = sizeof(info) / sizeof(Evas_Object*);
    p = &info.sep0;
    for (i = 0; i < n; i++)
      {
          Evas_Object        *o;

          o = p[i];
          evas_object_move( o, shell->w - real_width - 8,
                    i * (log_param.fontsize2 + 4) + 20);
      }
    evas_object_move( info.color, shell->w - real_width - 8, 35);
    evas_object_move( info.obj_color, shell->w - real_width - 8, 132);
}

void
info_sync(void)
{
    char                buf[4096];
    char              **strings;
    Evas               *e;
    Evas_Object       **p;
    int                 i, n;
    char                ch1, ch2;
    char               *s;

    e = shell->evas;

    evas_object_color_set(info.color, 
	shell->context.color.red*shell->context.color.alpha/255,
	shell->context.color.green*shell->context.color.alpha/255,
	shell->context.color.blue*shell->context.color.alpha/255, 
	shell->context.color.alpha);
    buf[0] = 0;
    if (shell->context.line_style)
        sprintf(buf, _("Line style: %s"), shell->context.line_style);
    s = my_iconv(shell->dcd, buf);
    evas_object_text_text_set(info.lstyle, s);
    IF_FREE(s);

    sprintf(buf, _("Line scale: %.2f"), shell->context.line_scale);
    s = my_iconv(shell->dcd, buf);
    evas_object_text_text_set(info.lscale, s);
    IF_FREE(s);

    sprintf(buf, _("Thickness: %.2f"), shell->context.thickness);
    s = my_iconv(shell->dcd, buf);
    evas_object_text_text_set(info.thickness, s);
    IF_FREE(s);

    sprintf(buf, _("Text style: %s"), shell->context.textstyle);
    s = my_iconv(shell->dcd, buf);
    evas_object_text_text_set(info.tstyle, s);
    IF_FREE(s);

    ch1 = 'C';
    if (shell->context.gravity & GRAVITY_L)
        ch1 = 'L';
    if (shell->context.gravity & GRAVITY_R)
        ch1 = 'R';
    ch2 = 'M';
    if (shell->context.gravity & GRAVITY_T)
        ch2 = 'T';
    if (shell->context.gravity & GRAVITY_B)
        ch2 = 'B';

    sprintf(buf, _("Gravity: %c%c"), ch1, ch2);
    s = my_iconv(shell->dcd, buf);
    evas_object_text_text_set(info.gravity, s);
    IF_FREE(s);

    sprintf(buf, _("Point style: %s"), shell->context.point_style);
    s = my_iconv(shell->dcd, buf);
    evas_object_text_text_set(info.pstyle, s);
    IF_FREE(s);

    {
        Color               col = common_get_color();

        evas_object_color_set(info.obj_color,
		col.red*col.alpha/255, 
		col.green*col.alpha/255, 
		col.blue*col.alpha/255, 
		col.alpha);
    }

    strings = common_get_info();

    for (i = 0; i < 10; i++)
      {
          s = my_iconv(shell->dcd, strings[i]);
          if (s && strlen(s) > 50)
              s[49] = 0;
          evas_object_text_text_set(info.obj_properties[i], s);

          IF_FREE(strings[i]);
          IF_FREE(s);
      }
    FREE(strings);

    n = sizeof(info) / sizeof(Evas_Object*);

    max_width = 0;
    p = &info.sep1;
    for (i = 0; i < n; i++)
      {
          int                 w;

          w = (int)evas_object_text_horiz_advance_get(p[i]);
          if (w > max_width)
              max_width = w;
      }
    real_width = max_width * width;
    info_configure();
}

void
info_sync_mouse(void)
{
    char                buf[4096];
    char               *s;
    char               *s1;
    int                 pr;
    Evas               *e;
    double              x, y, w;
    int                 _x, _y;
    int                 dummy;

    e = shell->evas;
    pr = shell->float_prec;

    ecore_x_pointer_xy_get(shell->win, &_x, &_y);

    x = s2w_x(_x);
    y = s2w_y(_y);

    magnet_get_xy(&dummy, &x, &y);

    s = _("Current: %.*f %.*f");
    s1 = my_iconv(shell->dcd, s);
    sprintf(buf, s1, pr, x, pr, y);
    evas_object_text_text_set(info.curr_xy, buf);
    IF_FREE(s1);

    w = evas_object_text_horiz_advance_get(info.curr_xy);
    if (max_width < w)
        info_configure();

    s = _("Previous: %.*f %.*f");
    s1 = my_iconv(shell->dcd, s);
    sprintf(buf, s1, pr, shell->context.fx, pr, shell->context.fy);
    IF_FREE(s1);
    evas_object_text_text_set(info.last_xy, buf);

    w = evas_object_text_horiz_advance_get(info.last_xy);
    if (max_width < w)
        info_configure();
}

void
info_toggle(void)
{
    if (width == 1.0)
      {
          info_hide();
      }
    else
      {
          info_show();
      }

}

int
info_anim_show(void *data)
{
    long val = (long)data;
    if (width < 1.0)
      {
          width = sin(M_PI/2/10*val);
          real_width = width * max_width;
          info_configure();
          ecore_timer_add(0.05, info_anim_show, (void*)(val + 1));
      }
    return 0;
}

void
info_show(void)
{
    info_anim_show(NULL);
    width = 0.0;
}

int
info_anim_hide(void *data)
{
    long val = (long)data;
    if (width >= 0.0)
      {
          width = sin(M_PI/2 - M_PI/2/10*val);
          real_width = width * max_width;
          info_configure();
          ecore_timer_add(0.05, info_anim_hide, (void*)(val + 1));
      }
    return 0;
}

void
info_hide(void)
{
    info_anim_hide(NULL);
    width = 1.0;
}


