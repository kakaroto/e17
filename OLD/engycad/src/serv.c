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
#include <pthread.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

/* vars */

pthread_mutex_t     queuemutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t     datamutex = PTHREAD_MUTEX_INITIALIZER;

pthread_t           thserv;
char               *ipc_script_name;
char                file[4096];

Eina_List          *serv_queue = NULL;

int                 parsed_flag;
int 		    fl_shutdown = FALSE;

/* protos */
char               *_serv_get_string(void);
void                serv_parser(char *);
void                serv_loop(void);
void                serv_run_script(void);

/** beans **/
void                serv_clone(void);
void                serv_move(void);
void                serv_rotate(void);
void                serv_mirror(void);
void                serv_trim(void);
void                serv_array(void);
void                serv_polar(void);
void                serv_scale(void);
void                serv_scale_xy(void);
void                serv_delete(void);
void                serv_copy(void);
void                serv_paste(void);
void                serv_save(void);
void                serv_save_as(void);
void                serv_open(void);
void                serv_save_image(void);
void                serv_save_ps(void);
void                serv_print(void);

void                serv_linestyle(void);
void                serv_color(void);
void                serv_thickness(void);
void                serv_linescale(void);
void                serv_gravity(void);
void                serv_text_height(void);
void                serv_text(void);
void                serv_image_scale(void);
void                serv_set_layer(void);
void                serv_help(void);

/* parsed-flag funcs */
void
clear_flag(void)
{
    parsed_flag = 0;
}
void
set_flag(void)
{
    parsed_flag = 1;
}
int
get_flag(void)
{
    return parsed_flag;
}

/* data-funcs */
void
lock_data(void)
{
    pthread_mutex_lock(&datamutex);
}
void
unlock_data(void)
{
    pthread_mutex_unlock(&datamutex);
}
int
trylock_data(void)
{
    return pthread_mutex_trylock(&datamutex);
}

/* serv funcs */
void
serv_init(void)
{
   // pthread_mutex_lock(&servmutex);
    pthread_create(&thserv, NULL, (void *)serv_loop, NULL);
};

void
_serv_put_string(char *s)
{
    if (!s)
        return;

    serv_queue = eina_list_append(serv_queue, s);
}

void
serv_put_string(char *s)
{
    char                buf[4096];
    int                 i, j;

    if (!s)
        return;
    if (strlen(s) > 4000)
        s[4000] = 0;

    pthread_mutex_lock(&queuemutex);
    for (i = 0, j = 0; s[i]; i++)
      {
          if ((s[i] == '|') || (j == 4095))
            {
                buf[j] = 0;
                _serv_put_string(DUP(_(buf)));
                j = 0;
            }
          else
            {
                buf[j] = s[i];
                j++;
            }
      }
    buf[j] = 0;
    _serv_put_string(DUP(_(buf)));
    FREE(s);
    pthread_mutex_unlock(&queuemutex);
}

char               *
_serv_get_string(void)
{
    char               *s = NULL;

    do
    {
	    pthread_mutex_lock(&queuemutex);
	    if(!serv_queue)
	    {
		    pthread_mutex_unlock(&queuemutex);
		    usleep(100*1000);
		    continue;
	    }
	    s = serv_queue->data;
	    serv_queue = eina_list_remove(serv_queue, s);
	    pthread_mutex_unlock(&queuemutex);
    }
    while(!s);

    return s;
}

char               *
serv_get_string(void)
{
    char               *s;

    s = _serv_get_string();
    if (s[0] == 0x27)
      {
          serv_parser(DUP(s + 1));
          FREE(s);
          return serv_get_string();
      }
    return s;
}

void
serv_set_hint(char *s)
{
    char               *buf;

    buf = (char *)malloc(strlen(s) + 100);
    sprintf(buf,"cl_set_hint|%s", s);
    gui_put_string(buf);
    FREE(s);
}

void
serv_loop(void)
{
    while (!fl_shutdown)
      {
          char               *s;

          serv_set_hint(DUP(_("cmd: ")));
          s = serv_get_string();
          serv_parser(s);
      }

    pthread_mutex_destroy(&queuemutex);
    pthread_mutex_destroy(&datamutex);
    fl_shutdown = 2;

}

#define IFCMD(a) if(!strcmp(_(s),_(a)))set_flag(); if(!strcmp(_(s),_(a)))
void
serv_parser(char *s)
{
    clear_flag();

    IFCMD("new_drawing") drawing_create();
    IFCMD("new_layer") layer_create();
    IFCMD("line") line_create();
    IFCMD("rectangle") rect_create();
    IFCMD("run_script") serv_run_script();
    IFCMD("point") point_create();
    IFCMD("circle") circle_create();
    IFCMD("arc") arc_create();
    IFCMD("ellipse") ellipse_create();
    IFCMD("earc") earc_create();
    IFCMD("image") image_create();
    IFCMD("text") text_create();
    IFCMD("dim_h") dim_h_create();
    IFCMD("dim_v") dim_v_create();
    IFCMD("dim_a") dim_a_create();
    IFCMD("hatch") hatch_create();

    IFCMD("start_sel") sel_rect();
    IFCMD("start_desel") desel_rect();
    IFCMD("select_all") common_all_sel();
    IFCMD("deselect_all") common_all_desel();
    IFCMD("inverse_selection") common_inv_sel();

    IFCMD("clone") serv_clone();
    IFCMD("move") serv_move();
    IFCMD("rotate") serv_rotate();
    IFCMD("mirror") serv_mirror();
    IFCMD("trim") serv_trim();
    IFCMD("array") serv_array();
    IFCMD("polar") serv_polar();

    IFCMD("scale") serv_scale();
    IFCMD("scale_xy") serv_scale_xy();

    IFCMD("set_line_style") serv_linestyle();
    IFCMD("set_color") serv_color();
    IFCMD("set_thickness") serv_thickness();
    IFCMD("set_line_scale") serv_linescale();

    IFCMD("set_gravity") serv_gravity();
    IFCMD("set_text_height") serv_text_height();
    IFCMD("set_text") serv_text();
    IFCMD("set_image_scale") serv_image_scale();

    IFCMD("set_layer") serv_set_layer();
    IFCMD("hide_layer")
    {
        lock_data();
// GLS        layer_hide(drawing->current_layer);
        unlock_data();
    }
    IFCMD("show_layer")
    {
        lock_data();
// GLS        layer_show(drawing->current_layer);
        unlock_data();
    }
    IFCMD("show_layers") layer_show_all();
    IFCMD("hide_layers") layer_hide_all();

    IFCMD("save") E_DB_FLUSH;
    IFCMD("save_as") serv_save_as();
    IFCMD("open") serv_open();
    IFCMD("copy") serv_copy();
    IFCMD("cut")
    {
        serv_copy();
        serv_delete();
    }
    IFCMD("paste") serv_paste();
    IFCMD("delete") serv_delete();

    IFCMD("save_image") serv_save_image();
    IFCMD("save_ps") serv_save_ps();
    IFCMD("help") serv_help();
    IFCMD("__exit") fl_shutdown = 1;
    FREE(s);
}

void
_serv_run_script_system(void)
{
    printf("Script started\n");
    system(ipc_script_name);
    printf("Script stoped\n");
}

void
_serv_run_script_reader(void)
{
    FILE               *f = NULL;
    char               *ff;

    ff = DUP(file);
    while (!f)
        f = fopen(ff, "r");
    ENGY_ASSERTS(f, ff);
    if (!f)
        return;
    while (!feof(f))
      {
          char                buf[4000];
          char               *s;

          fgets(buf, 4000, f);
          if (buf[strlen(buf) - 1] == '\n')
              buf[strlen(buf) - 1] = 0;
          gui_put_string(DUP(buf));
      }
    fclose(f);
    remove(ff);
    FREE(ff);
}

void
serv_run_script(void)
{
    char                buf[4096];
    pthread_t           thsys;
    pthread_t           threader;
    char               *name;
    char               *t;
    char               *s;

    serv_set_hint(DUP(_("enter script name: ")));
    name = serv_get_string();
    t = generate_name();
    s = getenv("TMPDIR");
    sprintf(buf, "%s/engycad-%s-in", s, t);
    FREE(t);
    strncpy(file, buf, strlen(buf));

    ENGY_ASSERT(!mknod(file, S_IFIFO | 0700, 0));

    s = malloc(strlen(name) + 100);
    sprintf(s, "%s > %s", name, file);
    ipc_script_name = (char *)DUP(s);
    FREE(s);
    FREE(name);

    pthread_create(&threader, NULL, (void *)_serv_run_script_reader, NULL);
    pthread_create(&thsys, NULL, (void *)_serv_run_script_system, NULL);
}

/****** change state  *******/

void
serv_set_state(int state)
{
    msg_create_and_send(CMD_SET_STATE, state, NULL);
}

/****** beans ******/

void
pre_set_x1y1(double x, double y)
{
    XY                 *o;
    Eina_List          *l = NULL;

    o = (XY *) malloc(sizeof(XY));
    ENGY_ASSERT(o);
    o->x = x;
    o->y = y;
    l = eina_list_append(l, o);
    msg_create_and_send(CMD_PRE_DATA, 0, l);
}

void
pre_set_x1y1x2y2(double x1, double y1, double x2, double y2)
{
    XY                 *o;
    Eina_List          *l = NULL;

    o = (XY *) malloc(sizeof(XY));
    ENGY_ASSERT(o);
    o->x = x1;
    o->y = y1;
    l = eina_list_append(l, o);

    o = (XY *) malloc(sizeof(XY));
    ENGY_ASSERT(o);
    o->x = x2;
    o->y = y2;
    l = eina_list_append(l, o);

    msg_create_and_send(CMD_PRE_DATA, 0, l);
}

void
serv_move(void)
{
    char               *s;
    float               x1, y1, x2, y2;
    int                 res;

    serv_set_hint(DUP(_("enter base point: ")));

    serv_set_state(ST_MOVE1);

    do
      {
          s = serv_get_string();
          res = get_values(s, shell->context.fx, shell->context.fy, &x1, &y1);
          if (res == 1)
              serv_set_hint(DUP(_("error, please reenter: ")));
      }
    while (res == 1);

    if (res == 0)
      {
          serv_set_state(ST_NORMAL);
          return;
      }

    serv_set_state(ST_MOVE2);

    pre_set_x1y1(x1, y1);

    serv_set_hint(DUP(_("enter insertion point: ")));
    do
      {
          s = serv_get_string();
          res = get_values(s, x1, y1, &x2, &y2);
          if (res == 1)
              serv_set_hint(DUP(_("error, please reenter: ")));
      }
    while (res == 1);

    serv_set_state(ST_NORMAL);

    if (res == 0)
        return;

    lock_data();
    common_move(x2 - x1, y2 - y1);
    shell->context.fx = x2;
    shell->context.fy = y2;
    unlock_data();
}

void
serv_clone(void)
{
    char               *s;
    float               x1, y1, x2, y2;
    int                 res;

    serv_set_hint(DUP(_("enter base point: ")));

    serv_set_state(ST_MOVE1);

    do
      {
          s = serv_get_string();
          res = get_values(s, shell->context.fx, shell->context.fy, &x1, &y1);
          if (res == 1)
              serv_set_hint(DUP(_("error, please reenter: ")));
      }
    while (res == 1);

    if (res == 0)
      {
          serv_set_state(ST_NORMAL);
          return;
      }

    serv_set_state(ST_MOVE2);

    pre_set_x1y1(x1, y1);

    serv_set_hint(DUP(_("enter insertion point: ")));
    do
      {
          s = serv_get_string();
          res = get_values(s, x1, y1, &x2, &y2);
          if (res == 1)
              serv_set_hint(DUP(_("error, please reenter: ")));
      }
    while (res == 1);

    serv_set_state(ST_NORMAL);

    if (res == 0)
        return;

    lock_data();
    common_clone(x2 - x1, y2 - y1);
    shell->context.fx = x2;
    shell->context.fy = y2;
    unlock_data();
}

void
serv_rotate(void)
{
    char               *s;
    float               x1, y1, x2, y2, angle;
    int                 res, res1 = 0;

    serv_set_hint(DUP(_("enter base point: ")));

    serv_set_state(ST_ROTATE1);

    do
      {
          s = serv_get_string();
          res = get_values(s, shell->context.fx, shell->context.fy, &x1, &y1);
          if (res == 1)
              serv_set_hint(DUP(_("error, please reenter: ")));
      }
    while (res == 1);

    if (res == 0)
      {
          serv_set_state(ST_NORMAL);
          return;
      }

    pre_set_x1y1(x1, y1);

    serv_set_hint(DUP(_("enter angle value: "))); /* rad (ex TODO rad vs deg)*/
    serv_set_state(ST_ROTATE2);
    do
      {
          s = serv_get_string();
          res = get_values(s, x1, y1, &x2, &y2);
          if (res == 2)
            {
                double              dx, dy;

                dx = x2 - x1;
                dy = y2 - y1;
                res1 = 1;
                if ((dy ==0) && (dx > 0))
                    angle = 0;
                if ((dy > 0) && (dx > 0))
                    angle = atan(dy/dx);
                if ((dy > 0) && (dx == 0))
                    angle = M_PI/2;
                if ((dy > 0) && (dx < 0))
                    angle = atan(dy/dx)+M_PI;
                if ((dy == 0) && (dx < 0))
                    angle = M_PI;
                if ((dy < 0) && (dx < 0))
                    angle = atan(dy/dx) + M_PI;
                if ((dy < 0) && (dx == 0))
                    angle = -M_PI / 2;
                if ((dy < 0) && (dx > 0))
                    angle = atan(dy / dx);
            }
          if (res == 1)
            {
                angle = x2 / 180 * M_PI;
                res1 = 1;
            }
          if (res == 0)
            {
                serv_set_state(ST_NORMAL);
                return;
            }
          if (res1 != 1)
              serv_set_hint(DUP(_("error, please reenter: ")));
      }
    while (res1 != 1);

    serv_set_state(ST_NORMAL);
    lock_data();
    common_rotate(x1, y1, angle);
    shell->context.fx = x1;
    shell->context.fy = y1;
    unlock_data();
}

void
serv_mirror(void)
{
    char               *s;
    float               x1, y1, x2, y2;
    int                 res;

    serv_set_hint(DUP(_("define first point of the mirroring line: ")));

    serv_set_state(ST_MIRROR1);

    do
      {
          s = serv_get_string();
          res = get_values(s, shell->context.fx, shell->context.fy, &x1, &y1);
          if (res == 1)
              serv_set_hint(DUP(_("error, please reenter: ")));
      }
    while (res == 1);

    if (res == 0)
      {
          serv_set_state(ST_NORMAL);
          return;
      }

    serv_set_state(ST_MIRROR2);

    pre_set_x1y1(x1, y1);

    serv_set_hint(DUP(_("define second point of the mirroring line: ")));

    do
      {
          s = serv_get_string();
          res = get_values(s, x1, y1, &x2, &y2);
          if (res == 1)
              serv_set_hint(DUP(_("error, please reenter: ")));
      }
    while (res == 1);

    serv_set_state(ST_NORMAL);

    if (res == 0)
        return;
    if ((x2 - x1 == 0) && (y2 - y1 == 0))
        return;
    lock_data();
    if (x2 - x1 == 0)
        common_mirror_x(x1);
    if (y2 - y1 == 0)
        common_mirror_y(y1);
    if ((x2 - x1 != 0) && (y2 - y1 != 0))
      {
          double              a, b;

          a = (y2 - y1) / (x2 - x1);
          b = y2 - a * x2;
          common_mirror_ab(a, b);
      };
    unlock_data();
}

void
serv_trim(void)
{
    char               *s;
    float               x1, y1, x2, y2, x3, y3;
    int                 res;

    serv_set_hint(DUP(_("define first point of the trimming line: ")));

    serv_set_state(ST_TRIM1);

    do
      {
          s = serv_get_string();
          res = get_values(s, shell->context.fx, shell->context.fy, &x1, &y1);
          if (res == 1)
              serv_set_hint(DUP(_("error, please reenter: ")));
      }
    while (res == 1);

    if (res == 0)
      {
          serv_set_state(ST_NORMAL);
          return;
      }

    serv_set_state(ST_TRIM2);

    pre_set_x1y1(x1, y1);

    serv_set_hint(DUP(_("define second point of the trimming line: ")));

    do
      {
          s = serv_get_string();
          res = get_values(s, x1, y1, &x2, &y2);
          if (res == 1)
              serv_set_hint(DUP(_("error, please reenter: ")));
      }
    while (res == 1);

    serv_set_state(ST_TRIM3);

    pre_set_x1y1x2y2(x1, y1, x2, y2);

    serv_set_hint(DUP(_("define side to be trimmed: ")));

    do
      {
          s = serv_get_string();
          res = get_values(s, x2, y2, &x3, &y3);
          if (res == 1)
              serv_set_hint(DUP(_("error, please reenter: ")));
      }
    while (res == 1);

    serv_set_state(ST_NORMAL);

    if (res == 0)
        return;

    if ((x2 - x1 == 0) && (y2 - y1 == 0))
        return;
    lock_data();
    if (x2 - x1 == 0)
        common_trim_x(x1, (int)(x3 > x1));
    if (y2 - y1 == 0)
        common_trim_y(y1, (int)(y3 > y1));
    if ((x2 - x1 != 0) && (y2 - y1 != 0))
      {
          double              a, b;

          a = (y2 - y1) / (x2 - x1);
          b = y2 - a * x2;
          common_trim_ab(a, b, (int)(y3 > (a * x3 + b)));
      };
    unlock_data();
}

void
serv_delete(void)
{
    lock_data();
    common_delete();
    unlock_data();
}

void
serv_copy(void)
{
    char               *s;
    float               x, y;
    int                 res;

    serv_set_hint(DUP(_("enter origin point: ")));

    serv_set_state(ST_LINE1);
    do
      {
          s = serv_get_string();
          res = get_values(s, shell->context.fx, shell->context.fy, &x, &y);
          if (res == 1)
              serv_set_hint(DUP(_("error, please reenter: ")));
      }
    while (res == 1);

    serv_set_state(ST_NORMAL);

    if (res == 0)
        return;

    lock_data();
    common_copy(x, y);
    unlock_data();
}

void
serv_paste(void)
{
    char               *s;
    float               x, y;
    int                 res;

    serv_set_hint(DUP(_("enter insertion point: ")));

    serv_set_state(ST_LINE1);
    do
      {
          s = serv_get_string();
          res = get_values(s, shell->context.fx, shell->context.fy, &x, &y);
          if (res == 1)
              serv_set_hint(DUP(_("error, please reenter: ")));
      }
    while (res == 1);

    serv_set_state(ST_NORMAL);

    if (res == 0)
        return;

    lock_data();
    common_paste(x, y);
    unlock_data();
}
void
serv_print(void)
{
    serv_save_image();
}

void
serv_save_as(void)
{
    char               *s;
    char                buf[4096];
    int                 res;

    serv_set_hint(DUP(_("enter new file name: ")));

    s = serv_get_string();
    if (strlen(s) < 2)
        return;

    sprintf(buf, "cp %s %s", drawing->fileout, s);
    res = system(buf);
    if ((res < 0))
      {
          log_add_string(DUP(_("Error while writing")));
          return;
      }
    if ((res == 127))
      {
          log_add_string(DUP(_("Error while writing")));
          return;
      }

    lock_data();
    drawing->fileout = s;
    unlock_data();
    FREE(s);
}

void
serv_open(void)
{
    char               *s;
    char                buf[4096];
    int                 res;

    serv_set_hint(DUP(_("enter file name for open: ")));

    s = serv_get_string();
    if (!s)
        return;
    if (strlen(s) < 2)
        return;
    if (s[strlen(s) - 1] != 'b')
        return;
    if (s[strlen(s) - 2] != 'd')
        return;
    if (s[strlen(s) - 3] != '.')
        return;

    sprintf(buf, "%s %s &", app_name, s);
    res = system(buf);
    FREE(s);
}

void
serv_linestyle(void)
{
    char               *s;
    char                buf[4096];
    int                 res;

    serv_set_hint(DUP(_("enter line style: ")));

    s = serv_get_string();

    lock_data();
    common_linestyle(s);
    unlock_data();
}

void
serv_color(void)
{
    char               *s;
    int                 r, g, b, a, res;

    serv_set_hint(DUP(_("enter color values: ")));
    s = serv_get_string();
    res = sscanf(s, "%d %d %d %d", &r, &g, &b, &a);
    lock_data();
    if (res == 4)
        common_color(r, g, b, a);
    unlock_data();
    FREE(s);
}
void
serv_thickness(void)
{
    float               w;
    char               *s;

    s = serv_get_string();
    w = (float)atof(s);
    lock_data();
    if (w != 0.0)
        common_thickness(w);
    unlock_data();
    FREE(s);
}
void
serv_linescale(void)
{
    float               w;
    char               *s;

    s = serv_get_string();
    w = atof(s);
    lock_data();
    if (w != 0.0)
        common_linescale(w);
    unlock_data();
    FREE(s);
}

void
serv_save_image(void)
{
    char               *s;

    serv_set_hint(DUP(_("enter image filename: ")));
    s = serv_get_string();
    msg_create_and_send(CMD_SAVE_IMAGE, 0, s);
}

void
serv_save_ps(void)
{
    char               *s;

    serv_set_hint(DUP(_("enter postscript filename: ")));
    s = serv_get_string();
    msg_create_and_send(CMD_SAVE_PS, 0, s);
}

void
serv_gravity(void)
{
    char               *s;
    int                 gr;

    serv_set_hint(DUP(_("enter gravity: ")));
    s = serv_get_string();

    if (strlen(s) != 2)
      {
          FREE(s);
          return;
      }

    gr = 0;
    switch (s[0])
      {
      case 'T':
          gr |= GRAVITY_T;
          break;
      case 'M':
          gr |= GRAVITY_M;
          break;
      case 'B':
          gr |= GRAVITY_B;
          break;
      case 'L':
          gr |= GRAVITY_L;
          break;
      case 'C':
          gr |= GRAVITY_C;
          break;
      case 'R':
          gr |= GRAVITY_R;
          break;
      }
    switch (s[1])
      {
      case 'T':
          gr |= GRAVITY_T;
          break;
      case 'M':
          gr |= GRAVITY_M;
          break;
      case 'B':
          gr |= GRAVITY_B;
          break;
      case 'L':
          gr |= GRAVITY_L;
          break;
      case 'C':
          gr |= GRAVITY_C;
          break;
      case 'R':
          gr |= GRAVITY_R;
          break;
      }

    FREE(s);
    lock_data();
    common_gravity(gr);
    unlock_data();
}

void
serv_text_height(void)
{
    char               *s;
    double              h;

    serv_set_hint(DUP(_("enter text height: ")));
    s = serv_get_string();

    h = atof(s);
    if (h == 0.0)
      {
          FREE(s);
          return;
      }

    FREE(s);
    lock_data();
    common_text_height(h);
    unlock_data();
}

void
serv_text(void)
{
    char               *s;
    double              h;

    serv_set_hint(DUP(_("enter text: ")));
    s = serv_get_string();

    lock_data();
    common_text(s);
    unlock_data();
}

void
serv_image_scale(void)
{
    char               *s;
    double              h;

    serv_set_hint(DUP(_("enter scale value: ")));
    s = serv_get_string();

    h = atof(s);
    FREE(s);
    if (h == 0.0)
        return;

    lock_data();
    common_image_scale(h);
    unlock_data();
}

void
serv_set_layer(void)
{
    char               *s;

    serv_set_hint(DUP(_("enter layer's name: ")));
    s = serv_get_string();

    lock_data();
    layer_set(s);
    unlock_data();
}

void
serv_array(void)
{
    char               *s, *s1, c;
    int                 i = 0;

    int                 n, m, res;
    float               dx, dy;

    serv_set_hint(DUP(_("Enter array's size (NxM): ")));
    do
      {
          s = serv_get_string();
          if (!strcmp(_(s), _("cancel")))
            {
                FREE(s);
                return;
            }
          if (!s)
              return;
          s1 = s;
          while ((++s1)[0])
              if (s1[0] == 'x')
                  s1[0] = ' ';
          c = (_("x"))[0];
          s1 = s;
          while ((++s1)[0])
              if (s1[0] == c)
                  s1[0] = ' ';

          res = sscanf(s, "%d %d", &n, &m);
          if (res != 2)
              serv_set_hint(DUP(_("error, please reenter: ")));
      }
    while (res != 2);

    serv_set_hint(DUP(_("Enter array's step (AxB): ")));
    do
      {
          s = serv_get_string();
          if (!strcmp(_(s), _("cancel")))
            {
                FREE(s);
                return;
            }
          if (!s)
              return;
          s1 = s;
          while ((++s1)[0])
              if (s1[0] == 'x')
                  s1[0] = ' ';
          c = (_("x"))[0];
          s1 = s;
          while ((++s1)[0])
              if (s1[0] == c)
                  s1[0] = ' ';

          res = sscanf(s, "%f %f", &dx, &dy);
          FREE(s);
          if (res != 2)
              serv_set_hint(DUP(_("error, please reenter: ")));
      }
    while (res != 2);

    lock_data();
    common_array(n, m, dx, dy);
    unlock_data();
}

void
serv_polar(void)
{
    char               *s;
    int                 num, res;
    float               x, y, da;

    serv_set_hint(DUP(_("enter base point: ")));

    serv_set_state(ST_LINE1);
    do
      {
          s = serv_get_string();
          res = get_values(s, shell->context.fx, shell->context.fy, &x, &y);
          if (res == 1)
              serv_set_hint(DUP(_("error, please reenter: ")));
      }
    while (res == 1);

    serv_set_state(ST_NORMAL);

    if (res == 0)
        return;

    serv_set_hint(DUP(_("enter number of copies: ")));
    s = serv_get_string();
    num = atof(s);
    FREE(s);
    if (num < 1)
        return;

    serv_set_hint(DUP(_("enter angle between items: ")));
    s = serv_get_string();
    da = atof(s);
    da = da / 180 * M_PI;
    FREE(s);
    if (da == 0)
        return;

    lock_data();
    common_polar(x, y, num, da);
    unlock_data();
}

void
serv_scale(void)
{
    char               *s;
    int                 num, res;
    float               x, y, sc;

    serv_set_hint(DUP(_("enter base point: ")));

    serv_set_state(ST_LINE1);
    do
      {
          s = serv_get_string();
          res = get_values(s, shell->context.fx, shell->context.fy, &x, &y);
          if (res == 1)
              serv_set_hint(DUP(_("error, please reenter: ")));
      }
    while (res == 1);

    serv_set_state(ST_NORMAL);

    if (res == 0)
        return;

    serv_set_hint(DUP(_("enter scale value: ")));
    s = serv_get_string();
    sc = atof(s);
    FREE(s);
    if (sc == 0)
        return;

    lock_data();
    common_scale(x, y, sc);
    unlock_data();
}

void
serv_scale_xy(void)
{
    char               *s;
    int                 num, res;
    float               x, y, scx, scy;

    serv_set_hint(DUP(_("enter base point: ")));

    serv_set_state(ST_LINE1);
    do
      {
          s = serv_get_string();
          res = get_values(s, shell->context.fx, shell->context.fy, &x, &y);
          if (res == 1)
              serv_set_hint(DUP(_("error, please reenter: ")));
      }
    while (res == 1);

    serv_set_state(ST_NORMAL);

    if (res == 0)
        return;

    serv_set_hint(DUP(_("enter X scale value: ")));
    s = serv_get_string();
    scx = atof(s);
    FREE(s);
    if (scx == 0)
        return;

    serv_set_hint(DUP(_("enter Y scale value: ")));
    s = serv_get_string();
    scy = atof(s);
    FREE(s);
    if (scy == 0)
        return;

    lock_data();
    common_scale_xy(x, y, scx, scy);
    unlock_data();
}

void
serv_help(void)
{
    char                buf[4096];
    char               *hb;

    hb = getenv("HELP_BROWSER");

    sprintf(buf, "%s %s/resources/help/index.html &", hb, shell->home);
    system(buf);
}
