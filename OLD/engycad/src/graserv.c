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
#include <Evas_Engine_Buffer.h>

/* vars */
pthread_mutex_t     gramutex = PTHREAD_MUTEX_INITIALIZER;
Evas_List          *gra_queue = NULL;

/*   pre_draw vars   */
Evas_List          *obj_data = NULL;
Evas_List          *pre_draw_objects = NULL;
int                 state = 0;

/* protos */
void                gra_redraw(Msg *);
void                gra_sync(Msg *);
void                gra_delete(Msg *);
void                gra_save_image(char *);
void                gra_save_ps(char *);

Msg                *
msg_create(void)
{
    Msg                *msg;

    msg = (Msg *) malloc(sizeof(Msg));
    ENGY_ASSERT(msg);
    memset(msg, 0, sizeof(Msg));
    return msg;
}

void
msg_create_and_send(int major, int minor, void *obj)
{
    Msg                *msg;

    msg = (Msg *) malloc(sizeof(Msg));
    ENGY_ASSERT(msg);
    memset(msg, 0, sizeof(Msg));
    msg->opcode.major = major;
    msg->opcode.minor = minor;
    msg->obj = obj;
    gra_put_msg(msg);
}

void
gra_put_msg(Msg * msg)
{
    pthread_mutex_lock(&gramutex);
    gra_queue = evas_list_append(gra_queue, msg);
    pthread_mutex_unlock(&gramutex);
}

int
gra_queue_empty(void)
{
    int                 res = 1;

    pthread_mutex_lock(&gramutex);
    if (gra_queue)
        res = 0;
    pthread_mutex_unlock(&gramutex);
    return res;
}

Msg                *
gra_get_msg(void)
{
    Msg                *msg;

    pthread_mutex_lock(&gramutex);
    msg = gra_queue->data;
    gra_queue = evas_list_remove(gra_queue, msg);
    pthread_mutex_unlock(&gramutex);
    return msg;
}

void
gra_apply(void)
{
//    if(trylock_data())return;
    lock_data();

    while (!gra_queue_empty())
      {
          Msg                *msg;

          msg = gra_get_msg();
          switch (msg->opcode.major)
            {
            case CMD_NONE:
                break;
            case CMD_REDRAW:
                gra_redraw(msg);
                break;
            case CMD_SYNC:
                gra_sync(msg);
                break;
            case CMD_DELETE:
                gra_delete(msg);
                break;
            case CMD_SET_STATE:
                gra_set_state(msg->opcode.minor);
                break;
            case CMD_PRE_DATA:
                gra_pre_draw_put_data(msg->obj);
                break;
            case CMD_INFO_SYNC:
                info_sync();
                break;
            case CMD_SAVE_IMAGE:
                gra_save_image(msg->obj);
                break;
            case CMD_SAVE_PS:
                gra_save_ps(msg->obj);
                break;

            default:
                ENGY_ASSERTS(0, "major default");
            }
          FREE(msg);
      }
    unlock_data();
}

void
gra_sync(Msg * msg)
{
    switch (msg->opcode.minor)
      {
      case OBJ_NONE:
          break;
      case OBJ_DRAWING:
          drawing_sync();
          break;
      case OBJ_LAYER:
          layer_sync((Layer *) msg->obj);
          break;
      case OBJ_POINT:
          point_sync((Point *) msg->obj);
          break;
      case OBJ_LINE:
          line_sync((Line *) msg->obj);
          break;
      case OBJ_CIRCLE:
          ci_sync((Circle *) msg->obj);
          break;
      case OBJ_ARC:
          arc_sync((Arc *) msg->obj);
          break;
      case OBJ_ELLIPSE:
          ell_sync((Ellipse *) msg->obj);
          break;
      case OBJ_EARC:
          earc_sync((EArc *) msg->obj);
          break;
      case OBJ_IMAGE:
          image_sync((Image *) msg->obj);
          break;
      case OBJ_TEXT:
          text_sync((Text *) msg->obj);
          break;
      case OBJ_MAGNET:
          break;
      default:
          ENGY_ASSERTS(0, "minor default");

      }
}

void
gra_redraw(Msg * msg)
{
    switch (msg->opcode.minor)
      {
      case OBJ_NONE:
          break;
      case OBJ_DRAWING:
          drawing_redraw();
          break;
      case OBJ_LAYER:
          layer_redraw((Layer *) msg->obj);
          break;
      case OBJ_POINT:
          point_redraw((Point *) msg->obj);
          break;
      case OBJ_LINE:
          line_redraw((Line *) msg->obj);
          break;
      case OBJ_CIRCLE:
          ci_redraw((Circle *) msg->obj);
          break;
      case OBJ_ARC:
          arc_redraw((Arc *) msg->obj);
          break;
      case OBJ_ELLIPSE:
          ell_redraw((Ellipse *) msg->obj);
          break;
      case OBJ_EARC:
          earc_redraw((EArc *) msg->obj);
          break;
      case OBJ_IMAGE:
          image_redraw((Image *) msg->obj);
          break;
      case OBJ_TEXT:
          text_redraw((Text *) msg->obj);
          break;
      case OBJ_MAGNET:
          magnet_redraw((MG *) msg->obj);
          break;
      default:
          ENGY_ASSERTS(0, "minor default");
      }
}

void
gra_delete(Msg * msg)
{
    switch (msg->opcode.minor)
      {
      case OBJ_NONE:
          break;
      case OBJ_DRAWING:
          break;
      case OBJ_LAYER:
          layer_destroy((Layer *) msg->obj);
          break;
      case OBJ_POINT:
          point_destroy((Point *) msg->obj);
          break;
      case OBJ_LINE:
          line_destroy((Line *) msg->obj);
          break;
      case OBJ_CIRCLE:
          ci_destroy((Circle *) msg->obj);
          break;
      case OBJ_ARC:
          arc_destroy((Arc *) msg->obj);
          break;
      case OBJ_EARC:
          earc_destroy((EArc *) msg->obj);
          break;
      case OBJ_ELLIPSE:
          break;
      default:
          ENGY_ASSERTS(0, "minor default");
      }
}

/*******  pre_draw  ***********/

void
gra_pre_draw_put_data(Evas_List *list)
{
    Evas_List           *l;

    for (l = obj_data; l; l = l->next)
        FREE(l->data);
    obj_data = evas_list_free(obj_data);

    obj_data = list;
}

void
gra_set_state(int newstate)
{
    if (newstate == state)
        return;
    switch (state)
      {
      case ST_NORMAL:
          break;
      case ST_DIM1:;
      case ST_LINE1:
          menu_item_reset("socket1", DUP(_(" Done ")), 0, NULL, DUP(_("done")));
          break;
      case ST_LINE2:
          ghost_line_destroy();
          menu_item_reset("socket1", DUP(_(" Line ")), 0, NULL, DUP(_("line")));
          break;
      case ST_RECT1:
          menu_item_reset("socket1", DUP(_(" Done ")), 0, NULL, DUP(_("done")));
          break;
      case ST_RECT2:
          ghost_rect_destroy();
          menu_item_reset("socket1", DUP(_(" Rect ")), 0, NULL, DUP(_("rect")));
          break;
      case ST_CIRCLE1:
          menu_item_reset("socket1", DUP(_(" Done ")), 0, NULL, DUP(_("done")));
          break;
      case ST_CIRCLE2:
          ghost_ci_destroy();
          menu_item_reset("socket1", DUP(_(" Circle ")),
                          0, NULL, DUP(_("circle")));
          break;
      case ST_ARC1:
          menu_item_reset("socket1", DUP(_(" Done ")), 0, NULL, DUP(_("done")));
          break;
      case ST_ELLIPSE1:
          menu_item_reset("socket1", DUP(_(" Done ")), 0, NULL, DUP(_("done")));
          break;
      case ST_ELLIPSE2:
          break;
      case ST_ELLIPSE3:
          ghost_ell_destroy();
          menu_item_reset("socket1", DUP(_(" Ellipse ")),
                          0, NULL, DUP(_("ell")));
          break;
      case ST_SEL1:
          break;
      case ST_SEL2:
          sel_box_destroy();
          break;
      case ST_MOVE1:;
      case ST_ROTATE1:;
      case ST_MIRROR1:;
      case ST_TRIM1:
          menu_item_reset("socket1", DUP(_(" Done ")),
                          0, NULL, DUP(_("done")));;
      case ST_MOVE2:
          ghost_line_destroy();
          menu_item_reset("socket1", DUP(_(" Move ")), 0, NULL, DUP(_("move")));
          break;
      case ST_ROTATE2:
          ghost_line_destroy();
          menu_item_reset("socket1", DUP(_(" Rotate ")),
                          0, NULL, DUP(_("rotate")));
          break;
      case ST_MIRROR2:
          ghost_line_destroy();
          menu_item_reset("socket1", DUP(_(" Mirror ")),
                          0, NULL, DUP(_("mirror")));
          break;
      case ST_TRIM2:
          ghost_line_destroy();
          break;
      case ST_TRIM3:
          menu_item_reset("socket1", DUP(_(" Trim ")), 0, NULL, DUP(_("trim")));
          break;
      case ST_DIM2:
          ghost_line_destroy();
          break;
      case ST_DIMV:;;
      case ST_DIMA:;;
      case ST_DIMH:;;
      case ST_DIMR:;;
      case ST_DIMD:;
          ghost_dim_destroy();

      }

    state = newstate;

    switch (state)
      {
      case ST_NORMAL:
          break;
      case ST_LINE1:
          break;
      case ST_LINE2:
          ghost_line_create();
          break;
      case ST_RECT1:
          break;
      case ST_RECT2:
          ghost_rect_create();
          break;
      case ST_CIRCLE1:
          break;
      case ST_CIRCLE2:
          ghost_ci_create();
          break;
      case ST_ELLIPSE1:
          break;
      case ST_ELLIPSE2:
          ghost_ell_create();
          break;
      case ST_ELLIPSE3:
          break;
      case ST_SEL1:
          break;
      case ST_SEL2:
          sel_box_create();
          break;
      case ST_MOVE1:
          break;
      case ST_ROTATE1:
          break;
      case ST_MIRROR1:
          break;
      case ST_TRIM1:
          break;
      case ST_MOVE2:;
      case ST_ROTATE2:;
      case ST_MIRROR2:;
      case ST_TRIM2:
          ghost_line_create();
          break;
      case ST_TRIM3:
          break;
      case ST_DIM1:
          break;
      case ST_DIM2:
          ghost_line_create();
          break;
      case ST_DIMH:;
      case ST_DIMV:;
      case ST_DIMA:;
      case ST_DIMR:;
      case ST_DIMD:
          ghost_dim_create();
          break;
      }
}

void
gra_pre_draw_mouse_move(double x, double y)
{
    int                 dummy;

    switch (state)
      {
      case ST_NORMAL:
          break;
      case ST_LINE1:
          break;
      case ST_DIM1:
          break;
      case ST_DIM2:;
      case ST_LINE2:
          magnet_get_xy(&dummy, &x, &y);
          ghost_line_redraw(obj_data, x, y);
          break;
      case ST_RECT1:
          break;
      case ST_RECT2:
          magnet_get_xy(&dummy, &x, &y);
          ghost_rect_redraw(obj_data, x, y);
          break;
      case ST_CIRCLE1:
          break;
      case ST_CIRCLE2:
          magnet_get_xy(&dummy, &x, &y);
          ghost_ci_redraw(obj_data, x, y);
          break;
      case ST_ELLIPSE1:
          break;
      case ST_ELLIPSE2:;
      case ST_ELLIPSE3:
          magnet_get_xy(&dummy, &x, &y);
          ghost_ell_redraw(obj_data, x, y);
          break;
      case ST_SEL1:
          break;
      case ST_SEL2:
          sel_box_redraw(obj_data, x, y);
          break;
      case ST_MOVE1:
          break;
      case ST_MOVE2:
          magnet_get_xy(&dummy, &x, &y);
          ghost_line_redraw(obj_data, x, y);
          break;
      case ST_ROTATE1:
          break;
      case ST_ROTATE2:
          magnet_get_xy(&dummy, &x, &y);
          ghost_line_redraw(obj_data, x, y);
          break;
      case ST_MIRROR1:
          break;
      case ST_MIRROR2:
          magnet_get_xy(&dummy, &x, &y);
          ghost_line_redraw(obj_data, x, y);
          break;
      case ST_TRIM1:
          break;
      case ST_TRIM2:
          magnet_get_xy(&dummy, &x, &y);
          ghost_line_redraw(obj_data, x, y);
          break;
      case ST_TRIM3:
          break;
      case ST_DIMH:
          ghost_dim_h_redraw(obj_data, x, y);
          break;
      case ST_DIMV:
          ghost_dim_v_redraw(obj_data, x, y);
          break;
      case ST_DIMA:
          ghost_dim_a_redraw(obj_data, x, y);
          break;
      }
}

void
gra_pre_draw_mouse_click(double x, double y)
{
    char               *s;
    char                buf[4096];
    int                 dummy;

    sprintf(buf, "");
    switch (state)
      {
      case ST_NORMAL:
          sel_click(x, y);
          break;
      case ST_DIM1:;
      case ST_DIM2:;
      case ST_DIMH:;
      case ST_DIMV:;
      case ST_DIMA:;
      case ST_DIMR:;
      case ST_DIMD:;
      case ST_LINE1:;
      case ST_LINE2:
          magnet_get_xy(&dummy, &x, &y);
          sprintf(buf, "%.*f %.*f", shell->float_prec, x, shell->float_prec, y);
          break;
      case ST_RECT1:;
      case ST_RECT2:
          magnet_get_xy(&dummy, &x, &y);
          sprintf(buf, "%.*f %.*f", shell->float_prec, x, shell->float_prec, y);
          break;
      case ST_POINT:
          magnet_get_xy(&dummy, &x, &y);
          sprintf(buf, "%.*f %.*f", shell->float_prec, x, shell->float_prec, y);
          break;
      case ST_CIRCLE1:;
      case ST_CIRCLE2:
          magnet_get_xy(&dummy, &x, &y);
          sprintf(buf, "%.*f %.*f", shell->float_prec, x, shell->float_prec, y);
          break;
      case ST_ELLIPSE1:;
      case ST_ELLIPSE2:;
      case ST_ELLIPSE3:
          magnet_get_xy(&dummy, &x, &y);
          sprintf(buf, "%.*f %.*f", shell->float_prec, x, shell->float_prec, y);
          break;
      case ST_SEL1:
          sprintf(buf, "%.*f %.*f", shell->float_prec, x, shell->float_prec, y);
          break;
      case ST_SEL2:
          sprintf(buf, "%.*f %.*f", shell->float_prec, x, shell->float_prec, y);
          break;
      case ST_MOVE1:;
      case ST_MOVE2:;
      case ST_ROTATE1:;
      case ST_ROTATE2:;
      case ST_MIRROR1:;
      case ST_MIRROR2:;
      case ST_TRIM1:;
      case ST_TRIM2:;
      case ST_TRIM3:
          magnet_get_xy(&dummy, &x, &y);
          sprintf(buf, "%.*f %.*f", shell->float_prec, x, shell->float_prec, y);
          break;
      }
    if (strlen(buf))
      {
          s = DUP(buf);
          gui_put_string(s);
      }
}

void
_best_fit(void)
{
    double              a, b, old;

    old = drawing->scale;
    a = shell->w / drawing->w;
    b = shell->h / drawing->h;

    drawing->scale = (a < b) ? a : b;
    a = (shell->w - drawing->w * drawing->scale) / 2;
    b = (shell->h - drawing->h * drawing->scale) / 2;
    a /= drawing->scale;
    b /= drawing->scale;
    drawing->x = a;
    drawing->y = b;
}

void
gra_save_image(char *file)
{

    Drawing            *d;
    Evas               *old_e, *e;
    double              old_res;
    Imlib_Image         im;
    int                 old_w, old_h;
    Evas_Coord          w, h;
    char                buf[4096];
    char	       *img_buf;

    d = drawing;
    if (!d)
      {
          FREE(file);
          return;
      }

    old_e = shell->evas;
    old_res = shell->monitor;
    old_w = shell->w;
    old_h = shell->h;

    w = d->w * shell->paper / unit(UNIT_INCH) * unit(d->unit);
    h = d->h * shell->paper / unit(UNIT_INCH) * unit(d->unit);

    w = (int)w;
    h = (int)h;

    sprintf(buf, "w=%d h=%d @ %d dpi", (int)w, (int)h, (int)shell->paper);
    log_add_string(DUP(buf));

    im = imlib_create_image((int)w, (int)h);
    imlib_context_set_image(im);
    imlib_image_set_has_alpha(1);
    imlib_image_clear();
    e = evas_new();
    evas_output_method_set(e, evas_render_method_lookup("buffer"));
    evas_output_viewport_set(e, 0, 0, w, h);
    evas_output_size_set(e, (int)w, (int)h);
    {
	    Evas_Engine_Info_Buffer *einfo;
	    einfo = (Evas_Engine_Info_Buffer *) evas_engine_info_get(e);

	    einfo->info.depth_type = EVAS_ENGINE_BUFFER_DEPTH_ARGB32;
	    img_buf = malloc((int)w * (int)h *4);
	    einfo->info.dest_buffer = img_buf;
	    einfo->info.dest_buffer_row_bytes = w*4;

	    evas_engine_info_set(e, (Evas_Engine_Info *) einfo);
    }

    shell->w = (int)w;
    shell->h = (int)h;
    shell->monitor = shell->paper;
    shell->evas = e;

    _best_fit();

    drawing_redraw();
    evas_render(e);

    {
	    char *p;
	    p = (char*) imlib_image_get_data();
	    memcpy(p, img_buf, (int)w * (int)h *4);
	    imlib_image_put_back_data( (DATA32*)p);
    }

    imlib_save_image(file);

    shell->w = old_w;
    shell->h = old_h;
    shell->evas = old_e;
    shell->monitor = old_res;

    _best_fit();
    drawing_sync();

    evas_free(e);
}

void
gra_save_ps(char *file)
{
    FILE               *f;

    f = fopen(file, "w");
    ENGY_ASSERTS(0, "saving ps");
    if (!f)
        return;

    shell->psout = f;
    drawing_redraw();
    shell->psout = NULL;

    fclose(f);
}
