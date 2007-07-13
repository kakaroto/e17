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

#include <stdlib.h>
#include "engy.h"
#include <pthread.h>

pthread_mutex_t     guimutex = PTHREAD_MUTEX_INITIALIZER;
extern Evas_Object * o_cl; // FIXME
Evas_List          *guiqueue = NULL;

/* protos */
void                set_show_thickness(char *s);
void                set_segnum(char *s);
void                set_arcnum(char *s);
void                set_linestyle(char *s);
void                set_color(char *s);
void                set_bg_color(char *s);
void                set_thickness(char *s);
void                set_linescale(char *s);
void                set_snap(char *);
void                set_prec(char *);
void                set_paper(char *);
void                set_monitor(char *);
void                set_text_height(char *s);
void                set_textstyle(char *s);
void                set_hatchstyle(char *s);
void                set_gravity(char *s);

void                shell_right(void);
void                shell_left(void);
void                shell_up(void);
void                shell_down(void);
void                shell_best_fit(void);

void
_gui_put_string(char *s)
{

    if (!s)
        return;
    pthread_mutex_lock(&guimutex);
    guiqueue = evas_list_append(guiqueue, s);
    pthread_mutex_unlock(&guimutex);
}

void
gui_put_string(char *s)
{
    char                buf[4096];
    int                 i, j;

    if (!s)
        return;
    if (strlen(s) > 4000)
        s[4000] = 0;

    for (i = 0, j = 0; s[i]; i++)
      {
          if ((s[i] == '|') || (j == 4000))
            {
                buf[j] = 0;
                _gui_put_string(DUP(_(buf)));
                j = 0;
            }
          else
            {
                buf[j] = s[i];
                j++;
            }
      }
    buf[j] = 0;
    _gui_put_string(DUP(_(buf)));
    FREE(s);
}

int
_gui_queue_empty(void)
{
    int                 res = 1;

    pthread_mutex_lock(&guimutex);
    if (guiqueue)
        res = 0;
    pthread_mutex_unlock(&guimutex);
    return res;
}

char               *
_gui_get_string(void)
{
    char               *s;

    pthread_mutex_lock(&guimutex);
    if (!guiqueue)
      {
          pthread_mutex_unlock(&guimutex);
          return NULL;
      }
    s = guiqueue->data;
    guiqueue = evas_list_remove(guiqueue, s);
    pthread_mutex_unlock(&guimutex);
    return s;
}

#define IFCMD(a) if(!strcmp(_(s),_(a)))fl=1; if(!strcmp(_(s),_(a)))

void
gui_apply(void)
{
    while (!_gui_queue_empty())
      {
          char               *s;
          int                 fl = 0, fl2 = 0;

          s = _gui_get_string();
          IFCMD("logo") logo_init();
          IFCMD("_exit") ecore_main_loop_quit();
	  // GLS
//          IFCMD("cl_toggle") cl_toggle();
          IFCMD("log_toggle") log_toggle();
          IFCMD("info_toggle") info_toggle();

          IFCMD("undo") apply_undo_backward();
          IFCMD("redo") apply_undo_forward();

          IFCMD("_zoom_in")
          {
              shell_zoom(shell->w / 2, shell->h / 2);
              fl2 = 1;
          }
          IFCMD("_zoom_out")
          {
              shell_unzoom(shell->w / 2, shell->h / 2);
              fl2 = 1;
          }
          IFCMD("_left")
          {
              shell_left();
              fl2 = 1;
          }
          IFCMD("_right")
          {
              shell_right();
              fl2 = 1;
          }
          IFCMD("_up")
          {
              shell_up();
              fl2 = 1;
          }
          IFCMD("_down")
          {
              shell_down();
              fl2 = 1;
          }
          IFCMD("_best_fit")
          {
              shell_best_fit();
              fl2 = 1;
          }

          IFCMD("cl_set_hint")
          {
	      char * hint = _gui_get_string();
              cl_hint_set(o_cl, hint);
	      FREE(hint);
              fl2 = 1;
          }
          IFCMD("_clean") evas_damage_rectangle_add(shell->evas, 0, 0, shell->w,
                                           shell->h);

          IFCMD("_set_show_thickness")
          {
              set_show_thickness(_gui_get_string());
              fl2 = 1;
          }
          IFCMD("_set_segnum")
          {
              set_segnum(_gui_get_string());
              fl2 = 1;
          }
          IFCMD("_set_arcnum")
          {
              set_arcnum(_gui_get_string());
              fl2 = 1;
          }
          IFCMD("_set_prec")
          {
              set_prec(_gui_get_string());
              fl2 = 1;
          }
          IFCMD("_set_monitor")
          {
              set_monitor(_gui_get_string());
              fl2 = 1;
          }
          IFCMD("_set_paper")
          {
              set_paper(_gui_get_string());
              fl2 = 1;
          }

          IFCMD("_set_line_style")
          {
              set_linestyle(_gui_get_string());
              fl2 = 1;
          }
          IFCMD("_set_color")
          {
              set_color(_gui_get_string());
              fl2 = 1;
          }
          IFCMD("_set_bg_color")
          {
              set_bg_color(_gui_get_string());
              fl2 = 1;
          }
          IFCMD("_set_thickness")
          {
              set_thickness(_gui_get_string());
              fl2 = 1;
          }
          IFCMD("_set_snap")
          {
              set_snap(_gui_get_string());
              fl2 = 1;
          }
          IFCMD("_set_line_scale")
          {
              set_linescale(_gui_get_string());
              fl2 = 1;
          }
          IFCMD("_set_hatch_style")
          {
              set_hatchstyle(_gui_get_string());
              fl2 = 1;
          }
          IFCMD("_set_text_style")
          {
              set_textstyle(_gui_get_string());
              fl2 = 1;
          }

          IFCMD("_set_gravity")
          {
              set_gravity(_gui_get_string());
              fl2 = 1;
          }
          IFCMD("_set_text_height")
          {
              set_text_height(_gui_get_string());
              fl2 = 1;
          }
          IFCMD("_set_layer")
          {
              lock_data();
              layer_set(_gui_get_string());
              unlock_data();
              fl2 = 1;
          };

          if (fl)
              info_sync();
          if (!fl2)
              log_add_string(DUP(s));
          if (!fl)
              serv_put_string(s);
	  else
	  {
	      FREE(s);
	  }
          if (!fl2)
              cl_hint_set(o_cl, _("cmd: "));
      }
}

/** beans **/

void
set_show_thickness(char *s)
{
    int                 n;

    n = atoi(s);
    lock_data();
    if (n == 0)
        shell->context.show_thickness = !shell->context.show_thickness;
    if (n == -1)
        shell->context.show_thickness = 0;
    if (n == 1)
        shell->context.show_thickness = 1;
    unlock_data();
    msg_create_and_send(CMD_REDRAW, OBJ_DRAWING, NULL);
    FREE(s);
    gui_put_string(DUP("_zoom_in|_zoom_out"));
}

void
set_segnum(char *s)
{
    int                 n;

    if (!s)
        return;
    n = atoi(s);
    if (n < 1)
        n = 1;
    lock_data();
    shell->segnum = n;
    unlock_data();
    msg_create_and_send(CMD_REDRAW, OBJ_DRAWING, NULL);
    FREE(s);
}

void
set_arcnum(char *s)
{
    int                 n;

    if (!s)
        return;
    n = atoi(s);
    if (n < 6)
        n = 6;
    lock_data();
    shell->arcnum = n;
    unlock_data();
    msg_create_and_send(CMD_REDRAW, OBJ_DRAWING, NULL);
    FREE(s);
}

void
set_prec(char *s)
{
    int                 n;

    if (!s)
        return;
    n = atoi(s);
    if (n < 0)
        n = 0;
    lock_data();
    shell->float_prec = n;
    unlock_data();
    FREE(s);
}

void
set_paper(char *s)
{
    double              n;

    if (!s)
        return;
    n = atof(s);
    if (n < 50)
        n = 96;
    lock_data();
    shell->paper = n;
    unlock_data();
    FREE(s);
}

void
set_monitor(char *s)
{
    double              n;

    if (!s)
        return;
    n = atof(s);
    if (n < 50)
        n = 96;
    lock_data();
    shell->monitor = n;
    unlock_data();
    FREE(s);
}

void
set_linestyle(char *s)
{
    int                 res, i = 0;
    char                buf[4096];

    if (!s)
        return;
    sprintf(buf, "/linestyle/%s/count", s);
    E_DB_INT_GET(shell->line_styles_file, buf, i, res);
    if ((!i) || (!res))
      {
          sprintf(buf, _("Line style '%s' not found!"), s);
          log_add_string(DUP(buf));
          FREE(s);
          return;
      }
    lock_data();
    FREE(shell->context.line_style);
    shell->context.line_style = s;
    unlock_data();
    sprintf(buf, "set_line_style|%s", s);
    serv_put_string(DUP(buf));
}

void
set_textstyle(char *s)
{
    int                 res, i = 0;
    char                buf[4096];

    if (!s)
        return;
    sprintf(buf, "/textstyle/%s/charmap", s);
    E_DB_INT_GET(shell->text_styles_file, buf, i, res);
    if ((!res))
      {
          sprintf(buf, _("Text style '%s' not found!"), s);
          log_add_string(DUP(buf));
          FREE(s);
          return;
      }
    lock_data();
    IF_FREE(shell->context.textstyle);
    shell->context.textstyle = s;
    unlock_data();
}

void
set_hatchstyle(char *s)
{
    char                buf[4096];
    int                 res, i = 0;

    if (!s)
        return;
    sprintf(buf, "/hatchstyle/%s/num", s);
    E_DB_INT_GET(shell->hatch_styles_file, buf, i, res);
    if ((!i) || (!res))
      {
          sprintf(buf, _("Hatch style '%s' not found!"), s);
          FREE(s);
          return;
      }
    lock_data();
    FREE(shell->context.hatch_style);
    shell->context.hatch_style = s;
    unlock_data();
}

void
set_color(char *s)
{
    int                 res, r, g, b, a;
    char                buf[4096];

    if (!s)
        return;
    res = sscanf(s, "%d %d %d %d", &r, &g, &b, &a);
    if (res != 4)
      {
          FREE(s);
          return;
      }

    lock_data();
    shell->context.color.red = r;
    shell->context.color.green = g;
    shell->context.color.blue = b;
    shell->context.color.alpha = a;
    unlock_data();
    sprintf(buf, "set_color|%d %d %d %d", r, g, b, a);
    serv_put_string(DUP(buf));
}

void
set_bg_color(char *s)
{
    int                 res, r, g, b, a;
    char                buf[4096];

    if (!s)
        return;
    res = sscanf(s, "%d %d %d %d", &r, &g, &b, &a);
    if (res != 4)
      {
          FREE(s);
          return;
      }

    lock_data();
    drawing->r = r;
    drawing->g = g;
    drawing->b = b;
    drawing->a = a;
    unlock_data();
    drawing_sync();
}

void
set_thickness(char *s)
{
    int                 res;
    char                buf[4096];
    double               w;

    if (!s)
        return;

    w = atof(s);

    if (w == 0.0)
      {
          FREE(s);
          return;
      }

    lock_data();
    shell->context.thickness = w;
    unlock_data();
    sprintf(buf, "set_thickness|%f", w);
    serv_put_string(DUP(buf));
}

void
set_snap(char *s)
{
    int                 res;
    char                buf[4096];
    float               w;

    if (!s)
        return;

    w = atof(s);

    if (w == 0)
      {
          FREE(s);
          return;
      }

    lock_data();
    shell->snap = w;
    unlock_data();
    FREE(s);
}

void
set_linescale(char *s)
{
    int                 res;
    char                buf[4096];
    float               w;

    if (!s)
        return;

    w = atof(s);

    if (w == 0)
      {
          FREE(s);
          return;
      }

    lock_data();
    shell->context.line_scale = w;
    unlock_data();
    sprintf(buf, "set_line_scale|%f", w);
    serv_put_string(DUP(buf));
}

void
set_gravity(char *s)
{
    int                 res;
    char                buf[4096];

    if (!s)
        return;

    if (strlen(s) != 2)
      {
          FREE(s);
          return;
      }

    res = 0;
    switch (s[0])
      {
      case 'T':
          res |= GRAVITY_T;
          break;
      case 'M':
          res |= GRAVITY_M;
          break;
      case 'B':
          res |= GRAVITY_B;
          break;
      case 'L':
          res |= GRAVITY_L;
          break;
      case 'C':
          res |= GRAVITY_C;
          break;
      case 'R':
          res |= GRAVITY_R;
          break;
      }
    switch (s[1])
      {
      case 'T':
          res |= GRAVITY_T;
          break;
      case 'M':
          res |= GRAVITY_M;
          break;
      case 'B':
          res |= GRAVITY_B;
          break;
      case 'L':
          res |= GRAVITY_L;
          break;
      case 'C':
          res |= GRAVITY_C;
          break;
      case 'R':
          res |= GRAVITY_R;
          break;
      }

    shell->context.gravity = res;
    sprintf(buf, "set_gravity|%s", s);
    FREE(s);
    serv_put_string(DUP(buf));
}

void
set_text_height(char *s)
{
    char                buf[4096];
    int                 res;
    double              h;

    if (!s)
        return;
    h = atof(s);
    if (h == 0.0)
      {
          FREE(s);
          return;
      }
    shell->context.text_height = h;
    sprintf(buf, "set_text_height|%f", h);
    FREE(s);
    serv_put_string(DUP(buf));
}

void
shell_right(void)
{
    if (!drawing)
        return;
    lock_data();
    drawing->x += 100.0 / drawing->scale;
    append_undo_double((void *)(&(drawing->x)),
                       drawing->old.x, drawing->x, 1, OBJ_DRAWING, NULL);
    unlock_data();
    drawing_sync();
}

void
shell_left(void)
{
    if (!drawing)
        return;
    lock_data();
    drawing->x -= 100.0 / drawing->scale;
    append_undo_double((void *)(&(drawing->x)),
                       drawing->old.x, drawing->x, 1, OBJ_DRAWING, NULL);
    unlock_data();
    drawing_sync();

}

void
shell_up(void)
{
    if (!drawing)
        return;
    lock_data();
    drawing->y += 100.0 / drawing->scale;
    append_undo_double((void *)(&(drawing->x)),
                       drawing->old.x, drawing->x, 1, OBJ_DRAWING, NULL);
    unlock_data();
    drawing_sync();
}

void
shell_down(void)
{
    if (!drawing)
        return;
    lock_data();
    drawing->y -= 100.0 / drawing->scale;
    append_undo_double((void *)(&(drawing->x)),
                       drawing->old.x, drawing->x, 1, OBJ_DRAWING, NULL);
    unlock_data();
    drawing_sync();
}

void
shell_best_fit(void)
{
    double              a, b, old;

    if (!drawing)
        return;
    lock_data();
    old = drawing->scale;
    a = shell->w / drawing->w;
    b = shell->h / drawing->h;

    drawing->scale = (a < b) ? a : b;
    append_undo_double((void *)(&(drawing->scale)),
                       old, drawing->scale, CMD_REDRAW, OBJ_DRAWING, NULL);

    a = (shell->w - drawing->w * drawing->scale) / 2;
    b = (shell->h - drawing->h * drawing->scale) / 2;
    a /= drawing->scale;
    b /= drawing->scale;

    append_undo_double((void *)(&(drawing->x)),
                       drawing->x, a, 1, OBJ_DRAWING, NULL);
    append_undo_double((void *)(&(drawing->y)), drawing->y, b, 0, 0, NULL);
    drawing->x = a;
    drawing->y = b;

    unlock_data();
    drawing_sync();
}
