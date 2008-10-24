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

#define IF_DR_SYNC(v) if(drawing->old.v != drawing->v)

/* vars */
Evas_Object         *o[7];       /* bg, shadow x2, border x4 */

/* protos */
Drawing            *_drawing_create(Unit, AUnit, double, double, double,
                                    double, double, double);
void                drawing_load_file(char *);

void
drawing_load(void)
{
    FILE               *f;
    char               *full, *fn;
    int                 new_file = 0;
    int                 read_only = 0;
    int                 i, res = 0;

    if (!shell->drawingfile)
        shell->drawingfile = _("default-drawing.db");
    full = DUP(shell->drawingfile);
    if (full[0] != '/')
      {
          char                buf[4096];
          char                wd[4096];

          getcwd(wd, sizeof(wd));
          if (wd[strlen(wd) - 1] == '/')
              wd[strlen(wd) - 1] = 0;
          sprintf(buf, "%s/%s", wd, full);
          FREE(full);
          full = DUP(buf);
      }
    fn = (char *)strrchr(shell->drawingfile, '/');
    if (fn)
        fn++;
    if (!fn)
        fn = shell->drawingfile;
    f = fopen(full, "rb");
    if (!f)
        new_file = 1;
    if (f)
        fclose(f);
    E_DB_INT_GET(full, "/unit", i, res);
    if (!res)
        new_file = 1;

    f = fopen(full, "a");
    if (!f)
        read_only = 1;
    if (f)
        fclose(f);

    if (new_file)
      {
          drawing = _drawing_create(-1, -1, -1, -1, -1, -1, -1, -1);
          gui_put_string(DUP("new_drawing"));
          drawing->filein = DUP(full);
      }
    else
      {
          drawing_load_file(DUP(full));
      }

    if (read_only)
      {
          char               *s1;
          char                buf[4096];
          int                 pid;

          s1 = getenv("TMPDIR");
	  ENGY_ASSERTS(s1, "*define $TMPDIR*!!!");
	  if(!s1) { sprintf(buf, "/tmp"); s1 = DUP(buf);}  
          pid = getpid();
          sprintf(buf, "%s/engycad-%d-%s", s1, pid, fn);
          drawing->fileout = DUP(buf);
	  FREE(s1);
      }
    else
      {
          drawing->fileout = DUP(full);
      }

    FREE(full);

    {
        char                buf[4096];

        if (strcmp(drawing->filein, drawing->fileout))
          {
              sprintf(buf, _(" %s (from readonly %s) -  %s"),
                      drawing->fileout, drawing->filein, TITLE);
          }
        else
          {
              sprintf(buf, _(" %s - %s"), drawing->fileout, TITLE);
          }
        ecore_evas_title_set(shell->ee, buf);
    }
}

void
drawing_free(void)
{
	Eina_List *l;

        for (l = drawing->layers; l; l = l->next)
	{
		layer_destroy((Layer *) l->data);
	}

	eina_list_free(drawing->layers);
	IF_FREE(drawing->filein);
	IF_FREE(drawing->fileout);
	FREE(drawing);
}

Unit
_read_unit(void)
{
    char               *s;
    int                 i, res;

    serv_set_hint(DUP(_("enter dimension units id: ")));
    s = serv_get_string();
    if (!strcmp(s, "") || !s)
      {
          E_DB_INT_GET(shell->rcfile, "/default/unit", i, res);
          ENGY_ASSERT(res);
          FREE(s);
          return i;
      }
    res = sscanf(s, "%d", &i);
    ENGY_ASSERT(res);
    FREE(s);
    return i;
}

AUnit
_read_aunit(void)
{
    char               *s;
    int                 i, res = 0;

    serv_set_hint(DUP(_("enter dimension angle units id: ")));
    s = serv_get_string();
    if (!strcmp(s, "") || !s)
      {
          E_DB_INT_GET(shell->rcfile, "/default/aunit", i, res);
          ENGY_ASSERT(res);
          FREE(s);
          return i;
      }
    res = sscanf(s, "%d", &i);
    ENGY_ASSERT(res);
    FREE(s);
    return i;
}

float
_read_dimscale(void)
{
    char               *s;
    int                 res;
    float               scale;

    serv_set_hint(DUP(_("enter plot scale: ")));
    s = serv_get_string();
    if (!strcmp(s, "") || !s)
      {
          E_DB_FLOAT_GET(shell->rcfile, "/default/dimscale", scale, res);
          ENGY_ASSERT(res);
          FREE(s);
          return scale;
      }
    res = sscanf(s, "%f", &scale);
    ENGY_ASSERT(res);
    FREE(s);
    return scale;
}

void
_read_size(double *w, double *h)
{
    char               *s;
    int                 res;
    float               a, b;

    serv_set_hint(DUP(_("enter sheet's width and height: ")));
    s = serv_get_string();
    if (!strcmp(s, "") || !s)
      {
          E_DB_FLOAT_GET(shell->rcfile, "/default/w", a, res);
          ENGY_ASSERT(res);
          E_DB_FLOAT_GET(shell->rcfile, "/default/h", b, res);
          ENGY_ASSERT(res);
          FREE(s);
          *w = a;
          *h = b;
          return;
      }
    res = sscanf(s, "%f %f", &a, &b);
    ENGY_ASSERT(res == 2);
    FREE(s);
    *w = a;
    *h = b;
}

void
drawing_create(void)
{
    char               *s;
    int                 unit;
    int                 aunit;
    double              dimscale, x, y, w, h, scale;

    unit = _read_unit();

    aunit = _read_aunit();

    dimscale = _read_dimscale();

    _read_size(&w, &h);

    x = (640.0 - w) / 2;
    y = (480.0 - h) / 2;
    scale = 1.0;

    lock_data();
    drawing = _drawing_create(unit, aunit, dimscale, x, y, w, h, scale);
    unlock_data();

    gui_put_string(DUP("_best_fit"));
    gui_put_string(DUP("_set_layer|Layer1"));

    msg_create_and_send(CMD_REDRAW, OBJ_DRAWING, NULL);
}

void
_drawing_check_evas_objects(void)
{
    Evas               *e;
    Drawing            *d;
    int                 i, res = 0;

    e = shell->evas;
    d = drawing;
    for (i = 0; i < 7; i++)
        if (!o[i])
            res = -1;

    if (d->old.evas && (e != d->old.evas))
      {
          double              x1, y1, w1, h1;
          Evas_Object        *o;

          o = evas_object_rectangle_add(e);
          evas_object_color_set(o, 
			  d->r*d->a/255, 
			  d->g*d->a/255, 
			  d->b*d->a/255, 
			  d->a);
          evas_object_layer_set(o, 5);
          x1 = (int)(d->x * d->scale);
          y1 = (int)(d->y * d->scale);
          w1 = (int)(d->w * d->scale);
          h1 = (int)(d->h * d->scale);
          evas_object_move(o, x1, shell->h - (y1 + h1));
          evas_object_resize(o, w1, h1);
          evas_object_show(o);
          return;
      }

    if (!res)
        return;

    for (i = 0; i < 7; i++)
        evas_object_del(o[i]);

    o[0] = evas_object_rectangle_add(e);

    o[1] = evas_object_rectangle_add(e);
    o[2] = evas_object_rectangle_add(e);

    o[3] = evas_object_line_add(e);
    o[4] = evas_object_line_add(e);
    o[5] = evas_object_line_add(e);
    o[6] = evas_object_line_add(e);

    evas_object_color_set(o[0], ALPHA5, ALPHA5, ALPHA5, ALPHA5);
    evas_object_color_set(o[1], 0, 0, 0, ALPHA2);
    evas_object_color_set(o[2], 0, 0, 0, ALPHA2);
    evas_object_color_set(o[3], 0, 0, 0, ALPHA5);
    evas_object_color_set(o[4], 0, 0, 0, ALPHA5);
    evas_object_color_set(o[5], 0, 0, 0, ALPHA5);
    evas_object_color_set(o[6], 0, 0, 0, ALPHA5);

    for (i = 0; i < 7; i++)
        evas_object_layer_set(o[i], 5);
    for (i = 0; i < 7; i++)
        evas_object_pass_events_set(o[i], 1);

    d->old.evas = shell->evas;
}

Drawing            *
_drawing_create(Unit unit,
                AUnit aunit,
                double ds, double x, double y, double w, double h, double sc)
{

    Drawing            *dr = drawing;

    if (!dr)
      {
          dr = (Drawing *) malloc(sizeof(Drawing));
          ENGY_ASSERT(dr);
          memset(dr, 0, sizeof(Drawing));
          dr->r = 255;
          dr->g = 255;
          dr->b = 255;
          dr->a = 255;
      }
    dr->dimscale = ds;
    dr->scale = sc;
    dr->unit = unit;
    dr->aunit = aunit;
    dr->x = x;
    dr->y = y;
    dr->w = w;
    dr->h = h;

    return dr;
}

void
drawing_redraw(void)
{
    Eina_List          *l;
    Evas               *e;
    Drawing            *d;
    Evas_Coord          x, y, w, h;
    Evas_Coord          x1, y1, w1, h1;
    int                 i;

    e = shell->evas;
    d = drawing;

    if (!e)
        return;
    if (!d)
        return;

    if (d->w <= 0)
        return;
    if (d->h <= 0)
        return;

    if (shell->psout);          /* TODO */

    _drawing_check_evas_objects();

    if (d->old.evas && (e != d->old.evas))
      {
          for (l = d->layers; l; l = l->next)
            {
                layer_redraw((Layer *) l->data);
            }
          return;
      };

    evas_output_viewport_get(e, &x, &y, &w, &h);
    x1 = (int)(d->x * d->scale);
    y1 = (int)(d->y * d->scale);
    w1 = (int)(d->w * d->scale);
    h1 = (int)(d->h * d->scale);

    evas_object_color_set(o[0], 
		    d->r*d->a/255, 
		    d->g*d->a/255, 
		    d->b*d->a/255,
		    d->a);
    evas_object_move(o[0], x1, h + y - (y1 + h1));
    evas_object_resize(o[0], w1, h1);

    evas_object_move(o[1], x1 + w1, h + y - (y1 + h1 - 4));
    evas_object_resize(o[1], 4, h1);

    evas_object_move(o[2], x1 + 4, h + y - y1);
    evas_object_resize(o[2], w1 - 4, 4);

    evas_object_line_xy_set(o[3], x1, h + y - y1, x1, h + y - y1 - h1);
    evas_object_line_xy_set(o[4], x1, h + y - y1, x1 + w1, h + y - y1);
    evas_object_line_xy_set(o[5], x1 + w1, h + y - y1, x1 + w1, h + y - y1 - h1);
    evas_object_line_xy_set(o[6], x1, h + y - y1 - h1, x1 + w1, h + y - y1 - h1);

    for (i = 0; i < 7; i++)
        evas_object_show(o[i]);

    for (l = d->layers; l; l = l->next)
      {
          layer_redraw((Layer *) l->data);
      }

    drawing->old.unit = drawing->unit;
    drawing->old.aunit = drawing->aunit;
    drawing->old.scale = drawing->scale;
    drawing->old.dimscale = drawing->dimscale;
    drawing->old.r = drawing->r;
    drawing->old.g = drawing->g;
    drawing->old.b = drawing->b;
    drawing->old.a = drawing->a;
    drawing->old.x = drawing->x;
    drawing->old.y = drawing->y;
    drawing->old.w = drawing->w;
    drawing->old.h = drawing->h;
}

void
drawing_sync(void)
{
    Eina_List          *l;

    IF_DR_SYNC(unit) E_DB_INT_SET(drawing->fileout, "/unit", drawing->unit);
    IF_DR_SYNC(aunit) E_DB_INT_SET(drawing->fileout, "/aunit", drawing->aunit);
    IF_DR_SYNC(r) E_DB_INT_SET(drawing->fileout, "/r", drawing->r);
    IF_DR_SYNC(g) E_DB_INT_SET(drawing->fileout, "/g", drawing->g);
    IF_DR_SYNC(b) E_DB_INT_SET(drawing->fileout, "/b", drawing->b);
    IF_DR_SYNC(a) E_DB_INT_SET(drawing->fileout, "/a", drawing->a);

    IF_DR_SYNC(scale) E_DB_FLOAT_SET(drawing->fileout, "/scale",
                                     drawing->scale);
    IF_DR_SYNC(dimscale) E_DB_FLOAT_SET(drawing->fileout, "/dimscale",
                                        drawing->dimscale);
    IF_DR_SYNC(x) E_DB_FLOAT_SET(drawing->fileout, "/x", drawing->x);
    IF_DR_SYNC(y) E_DB_FLOAT_SET(drawing->fileout, "/y", drawing->y);
    IF_DR_SYNC(w) E_DB_FLOAT_SET(drawing->fileout, "/w", drawing->w);
    IF_DR_SYNC(h) E_DB_FLOAT_SET(drawing->fileout, "/h", drawing->h);
    drawing_redraw();
}

void
drawing_load_file(char *fn)
{
    int                 res = 0;
    Drawing            *d;
    float               dummy;
    int                 i;

    if (drawing)
        return;

    drawing = _drawing_create(4, 0, 1, 0, 0, 210, 297, 1);
    drawing->filein = fn;
    d = drawing;

    E_DB_INT_GET(fn, "/r", d->r, res);
    ENGY_ASSERT(res);
    E_DB_INT_GET(fn, "/g", d->g, res);
    ENGY_ASSERT(res);
    E_DB_INT_GET(fn, "/b", d->b, res);
    ENGY_ASSERT(res);
    E_DB_INT_GET(fn, "/a", d->a, res);
    ENGY_ASSERT(res);

    E_DB_INT_GET(fn, "/unit", i, res);
    ENGY_ASSERT(res);
    d->unit = i;
    E_DB_INT_GET(fn, "/aunit", i, res);
    ENGY_ASSERT(res);
    d->aunit = i;

    E_DB_FLOAT_GET(fn, "/x", dummy, res);
    ENGY_ASSERT(res);
    d->x = dummy;
    E_DB_FLOAT_GET(fn, "/y", dummy, res);
    ENGY_ASSERT(res);
    d->y = dummy;
    E_DB_FLOAT_GET(fn, "/w", dummy, res);
    ENGY_ASSERT(res);
    d->w = dummy;
    E_DB_FLOAT_GET(fn, "/h", dummy, res);
    ENGY_ASSERT(res);
    d->h = dummy;
    E_DB_FLOAT_GET(fn, "/scale", dummy, res);
    ENGY_ASSERT(res);
    d->scale = dummy;
    E_DB_FLOAT_GET(fn, "/dimscale", dummy, res);
    ENGY_ASSERT(res);
    d->dimscale = dummy;

    {
        E_DB_File          *edb = NULL;
        char              **layers;
        int                 num, i;

        edb = e_db_open_read(drawing->filein);
        ENGY_ASSERT(edb);

        layers = e_db_match_keys(edb, "/drawing/layer/*", &num);
        e_db_close(edb);

        for (i = 0; i < num; i++)
          {
              int                 id;
              char               *s;

              s = (char *)strrchr(layers[i], '/');
              ENGY_ASSERT(s);
              id = atoi(s + 1);
              ENGY_ASSERT(id);
              IF_FREE(layers[i]);
              layer_load(id);
          }
        FREE(layers);

    }
    drawing_redraw();
}
