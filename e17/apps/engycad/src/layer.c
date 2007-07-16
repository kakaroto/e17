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

#define IF_LR_SYNC(a) if(layer->old.a != layer->a)
#define IF_LR_SYNC_S(a) if(!layer->a || !layer->old.a \
		|| !strcmp(layer->old.a,layer->a))

/* proto */
void                _layer_create(char *, int, int, int, int, double, char *);
void                layer_ssync(Layer *);
void                layer_menu_handling(void);

void
layer_create(void)
{
    Msg                *msg;
    Layer              *layer;
    char               *lab;
    char               *line_style;
    char               *s;
    int                 r, g, b, a, res = 0;
    float               width;

    serv_set_hint(DUP(_("enter layer's label: ")));
    lab = serv_get_string();
    do
      {
          serv_set_hint(DUP(_("enter layer's color: ")));
          s = serv_get_string();
          res = sscanf(s, "%d %d %d %d", &r, &g, &b, &a);
          FREE(s);
      }
    while (res != 4);

    serv_set_hint(DUP(_("enter layer's line thickness: ")));
    s = serv_get_string();
    width = 0.2;
    sscanf(s, "%f", &width);

    serv_set_hint(DUP(_("enter layer's line style: ")));
    line_style = serv_get_string();

    lock_data();
    _layer_create(lab, r, g, b, a, width, line_style);
    unlock_data();

    msg = (Msg *) malloc(sizeof(Msg));
    ENGY_ASSERT(msg);
    memset(msg, 0, sizeof(Msg));
    msg->opcode.major = 1;
    msg->opcode.minor = OBJ_LAYER;

    gra_put_msg(msg);
}

Layer              *
_layer_create_copy(Layer * src)
{
    Layer              *layer;

    layer = (Layer *) malloc(sizeof(Layer));
    ENGY_ASSERT(layer);
    memset(layer, 0, sizeof(Layer));

    layer->type = OBJ_LAYER;
    layer->id = rand();
    layer->flags = FLAG_VISIBLE;
    layer->label = generate_name();

    if (!src)
        return layer;
    layer->flags = src->flags;
    layer->color = src->color;
    layer->line_style = DUP(src->line_style);
    layer->thickness = src->thickness;
    layer->line_scale = src->line_scale;

    return layer;
}

void
_layer_create(char *label, int r, int g, int b, int a,
              double width, char *line_style)
{
    Layer              *layer;

    layer->type = OBJ_LAYER;
    layer = _layer_create_copy(NULL);
    layer->label = label;

    layer->color.red = r;
    layer->color.green = g;
    layer->color.blue = b;
    layer->color.alpha = a;
    layer->thickness = width;
    layer->line_style = line_style;

    append_undo_new_object(layer, CMD_REDRAW, OBJ_LAYER, layer);

    drawing->layers = evas_list_append(drawing->layers, layer);
    layer_set(layer->label);
    layer_ssync(layer);
    layer_sync(layer);
    layer_menu_handling();
}

void
layer_set(char *label)
{
    Evas_List          *l;
    Layer              *layer = NULL;
    Drawing            *d;

    d = drawing;

    if (!d)
        return;
    if (!label)
        return;
    for (l = d->layers; l && !layer; l = l->next)
      {
          Layer              *layer1;

          layer1 = (Layer *) l->data;
          if (!strcmp(layer1->label, label))
              layer = layer1;
      }
    if (!layer)
      {
          layer = _layer_create_copy(NULL);
          IF_FREE(layer->label);
          layer->label = DUP(label);
          layer->color = shell->context.color;
          layer->line_style = DUP(shell->context.line_style);
          layer->line_scale = shell->context.line_scale;
          layer->thickness = shell->context.thickness;
          layer_ssync(layer);
          layer_sync(layer);
          append_undo_new_object(layer, CMD_REDRAW, OBJ_LAYER, layer);
          drawing->layers = evas_list_append(drawing->layers, layer);
      }
    FREE(label);
    append_undo_long((void *)&(drawing->current_layer),
                    (long)drawing->current_layer,
                    (long)layer, CMD_REDRAW, OBJ_DRAWING, NULL);
    drawing->current_layer = layer;
    layer_menu_handling();
}

void
layer_redraw(Layer * layer)
{
    Evas_List          *l;

    if (!layer)
        return;
    for (l = layer->objects; l; l = l->next)
      {
          Object             *o;

          o = (Object *) l->data;
          switch (o->type)
            {
            case OBJ_NONE:
                break;
            case OBJ_POINT:
                point_redraw((Point *) o);
                break;
            case OBJ_LINE:
                line_redraw((Line *) o);
                break;
            case OBJ_CIRCLE:
                ci_redraw((Circle *) o);
                break;
            case OBJ_ARC:
                arc_redraw((Arc *) o);
                break;
            case OBJ_ELLIPSE:
                ell_redraw((Ellipse *) o);
                break;
            case OBJ_EARC:
                earc_redraw((EArc *) o);
                break;
            case OBJ_IMAGE:
                image_redraw((Image *) o);
                break;
            case OBJ_TEXT:
                text_redraw((Text *) o);
                break;
            }
      }

    layer->old.flags = layer->flags;
    if (!layer->old.label || strcmp(layer->old.label, layer->label))
      {
          IF_FREE(layer->old.label);
          layer->old.label = DUP(layer->label);
      }
}

void
layer_sync(Layer * layer)
{
    Evas_List          *l;
    char               *f;
    char                buf[4096];

    if (!drawing)
        return;

    f = drawing->fileout;

    IF_LR_SYNC(flags)
    {
        sprintf(buf, "/layer/%d/flags", layer->id);
        E_DB_INT_SET(f, buf, layer->flags);
    }
    IF_LR_SYNC_S(label)
    {
        sprintf(buf, "/layer/%d/label", layer->id);
        E_DB_STR_SET(f, buf, layer->label);
    }
    layer_redraw(layer);
}

void
layer_destroy(Layer * layer)
{
    Evas_List          *l;

    if (!layer)
        return;
    for (l = layer->objects; l; l = l->next)
      {
          Object             *o;

          o = (Object *) l->data;
          switch (o->type)
            {
            case OBJ_NONE:
                break;
            case OBJ_POINT:
                point_free((Point *) o);
                break;
            case OBJ_LINE:
                line_free((Line *) o);
                break;
	    case OBJ_CIRCLE:
		ci_free((Circle *) o);
		break;
	    case OBJ_ARC:
		arc_free((Arc *) o);
		break;
	    case OBJ_ELLIPSE:
		ell_free((Ellipse *) o);
		break;
	    case OBJ_EARC:
		earc_free((EArc *) o);
		break;
	    case OBJ_IMAGE:
		image_free((Image *) o);
	    case OBJ_TEXT:
		text_free((Text *) o);
		break;
            }
      }

    evas_list_free(layer->objects);
    FREE(layer->line_style);
    FREE(layer->label);
    IF_FREE(layer->old.line_style);
    IF_FREE(layer->old.label);
    FREE(layer);
}

void
layer_ssync(Layer * layer)
{
    char                buf[4096];
    char               *f;

    f = drawing->fileout;

    sprintf(buf, "/drawing/layer/%d", layer->id);
    E_DB_INT_SET(f, buf, layer->id);

    sprintf(buf, "/layer/%d/r", layer->id);
    E_DB_INT_SET(f, buf, layer->color.red);

    sprintf(buf, "/layer/%d/g", layer->id);
    E_DB_INT_SET(f, buf, layer->color.green);

    sprintf(buf, "/layer/%d/b", layer->id);
    E_DB_INT_SET(f, buf, layer->color.blue);

    sprintf(buf, "/layer/%d/a", layer->id);
    E_DB_INT_SET(f, buf, layer->color.alpha);

    sprintf(buf, "/layer/%d/thickness", layer->id);
    E_DB_FLOAT_SET(f, buf, layer->thickness);

    sprintf(buf, "/layer/%d/linestyle", layer->id);
    E_DB_STR_SET(f, buf, layer->line_style);
}

void
layer_load(int id)
{
    Layer              *layer;
    char                buf[4096];
    float               x;
    char               *f;
    int                 res;

    f = drawing->filein;

    layer = _layer_create_copy(NULL);
    layer->id = id;

    sprintf(buf, "/layer/%d/flags", layer->id);
    E_DB_INT_GET(f, buf, layer->flags, res);
    ENGY_ASSERT(res);

    sprintf(buf, "/layer/%d/label", layer->id);
    IF_FREE(layer->label);
    E_DB_STR_GET(f, buf, layer->label, res);
    ENGY_ASSERT(res);
    if(layer->label && (strlen(layer->label)>4000))
	                layer->label[4000]=0;

    sprintf(buf, "/layer/%d/r", layer->id);
    E_DB_INT_GET(f, buf, layer->color.red, res);
    ENGY_ASSERT(res);

    sprintf(buf, "/layer/%d/g", layer->id);
    E_DB_INT_GET(f, buf, layer->color.green, res);
    ENGY_ASSERT(res);

    sprintf(buf, "/layer/%d/b", layer->id);
    E_DB_INT_GET(f, buf, layer->color.blue, res);
    ENGY_ASSERT(res);

    sprintf(buf, "/layer/%d/a", layer->id);
    E_DB_INT_GET(f, buf, layer->color.alpha, res);
    ENGY_ASSERT(res);

    sprintf(buf, "/layer/%d/linestyle", layer->id);
    E_DB_STR_GET(f, buf, layer->line_style, res);
    ENGY_ASSERT(res);
    if(layer->line_style && (strlen(layer->line_style)>4000))
	                layer->line_style[4000]=0;

    sprintf(buf, "/layer/%d/thickness", layer->id);
    E_DB_FLOAT_GET(f, buf, x, res);
    layer->thickness = x;
    ENGY_ASSERT(res);

    drawing->layers = evas_list_append(drawing->layers, layer);
    layer_set(DUP(layer->label));

    {
        E_DB_File          *edb = NULL;
        char              **obj;
        char                buf[4096];
        int                 num, i;

        edb = e_db_open_read(f);
        ENGY_ASSERT(edb);
        sprintf(buf, "/layer/%d/entity/*", id);
        obj = e_db_match_keys(edb, buf, &num);
        e_db_close(edb);

        for (i = 0; i < num; i++)
          {
              int                 id;
              char               *s;
              int                 type, res;

              s = (char *)strrchr(obj[i], '/');
              ENGY_ASSERT(s);
              id = atoi(s + 1);
              ENGY_ASSERT(id);
              E_DB_INT_GET(f, obj[i], type, res);
              common_entity_load(id, type);
              IF_FREE(obj[i]);
          }
        FREE(obj);
    }
}

void
layer_menu_handling(void)
{
    int                 i = 0;
    Evas_List          *l;

    for (l = drawing->layers; l && (i < 20); l = l->next)
      {
          Layer              *layer;
          char                buf[4096];
          char               *s1, *s2, *s3;
          int                 ac = 0;

          layer = (Layer *) l->data;
          if (layer->flags & FLAG_DELETED)
              ac = 4;
          sprintf(buf, "layer%d", i);
          s1 = DUP(buf);
          sprintf(buf, _(" %s "), layer->label);
          s2 = DUP(buf);
          sprintf(buf, "set_layer|%s", layer->label);
          s3 = DUP(buf);
          menu_item_reset(s1, s2, ac, NULL, s3);
          FREE(s1);
          i++;
      }
}

void
layer_hide(Layer * layer)
{
    Evas_List          *l;

    for (l = layer->objects; l; l = l->next)
      {
          Object             *o;

          o = (Object *) l->data;
          if (o->flags & FLAG_VISIBLE)
              o->flags -= FLAG_VISIBLE;
      }
    layer_redraw(layer);
}

void
layer_show(Layer * layer)
{
    Evas_List          *l;

    for (l = layer->objects; l; l = l->next)
      {
          Object             *o;

          o = (Object *) l->data;
          if (!(o->flags & FLAG_VISIBLE))
              o->flags |= FLAG_VISIBLE;
      }
    layer_redraw(layer);
}

void
layer_hide_all(void)
{
    Evas_List          *l;

    if (!drawing)
        return;
    lock_data();
    for (l = drawing->layers; l; l = l->next)
      {
          layer_hide((Layer *) l->data);
      }
    unlock_data();
}

void
layer_show_all(void)
{
    Evas_List          *l;

    if (!drawing)
        return;
    lock_data();
    for (l = drawing->layers; l; l = l->next)
      {
          layer_show((Layer *) l->data);
      }
    unlock_data();
}
