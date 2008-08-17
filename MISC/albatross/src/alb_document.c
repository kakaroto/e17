/* alb_document.c

Copyright (C) 1999,2000 Tom Gilbert.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies of the Software and its documentation and acknowledgment shall be
given in the documentation and software packages that this Software was
used.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#include "alb_document.h"
#include "alb_thumb.h"
#include "alb_document_xml.h"

alb_document *
alb_document_new(int w,
                 int h)
{
  alb_document *d = NULL;

  D_ENTER(3);

  d = emalloc(sizeof(alb_document));

  memset(d, 0, sizeof(alb_document));

  d->w = w;
  d->h = h;

  d->im = imlib_create_image(w, h);
  d->pmap = XCreatePixmap(disp, root, w, h, depth);
  d->bg_fill = alb_fill_new_coloured(255, 255, 255, 255);
  d->name = estrdup("New document");
  alb_document_add_layer(d);

  doc_list = gib_list_add_end(doc_list, d);

  D_RETURN(3, d);
}

void
alb_document_free(alb_document * document)
{
  gib_list *l;

  D_ENTER(3);

  for (l = doc_list; l; l = l->next) {
    if (ALB_DOCUMENT(l->data) == document) {
      doc_list = gib_list_unlink(doc_list, l);
    }
  }

  for (l = document->layers; l; l = l->next)
    alb_layer_free((alb_layer *) l->data);

  gib_list_free(document->layers);

  if (document->im)
    gib_imlib_free_image(document->im);
  if (document->pmap)
    XFreePixmap(disp, document->pmap);

  efree(document);

  D_RETURN_(3);
}

void
alb_document_render(alb_document * document)
{
  gib_list *l;

  D_ENTER(3);

  gib_imlib_image_fill_rectangle(document->im, 0, 0, document->w, document->h,
                                 255, 255, 255, 255);

  alb_fill_render(document->bg_fill, document->im, 0, 0, document->w,
                  document->h);

  for (l = document->layers; l; l = l->next)
    alb_layer_render((alb_layer *) l->data, document->im);

  D_RETURN_(3);
}

gib_list *
alb_document_get_selected_list(alb_document * doc)
{
  gib_list *l, *ll, *ret = NULL;
  alb_object *obj;

  D_ENTER(3);

  for (l = doc->layers; l; l = l->next) {
    for (ll = ALB_LAYER(l->data)->objects; ll; ll = ll->next) {
      obj = ALB_OBJECT(ll->data);

      if (alb_object_get_state(obj, SELECTED)) {
        D(5, ("selected object found\n"));
        ret = gib_list_add_end(ret, obj);
      }
    }
  }
  D_RETURN(3, ret);
}

void
alb_document_unselect_all(alb_document * doc)
{
  gib_list *sl, *l;

  D_ENTER(3);

  sl = alb_document_get_selected_list(doc);
  D(3, ("selected items count: %d\n", gib_list_length(sl)));
  if (sl) {
    alb_object *obj;

    for (l = sl; l; l = l->next) {
      obj = ALB_OBJECT(l->data);
      if (alb_object_get_state(obj, SELECTED)) {
        alb_object_unset_state(obj, SELECTED);
        alb_object_dirty(obj);
      }
    }
    gib_list_free(sl);
  }

  D_RETURN_(3);
}

void
alb_document_render_selection(alb_document * doc)
{
  gib_list *sl, *l;
  int sel_count;

  D_ENTER(3);

  sl = alb_document_get_selected_list(doc);
  D(3, ("selected items count: %d\n", gib_list_length(sl)));

  if (sl) {
    alb_object *obj;

    sel_count = gib_list_length(sl);

    for (l = sl; l; l = l->next) {
      obj = ALB_OBJECT(l->data);
      obj->render_selected(obj, doc->im, (sel_count > 1) ? TRUE : FALSE);
    }
    gib_list_free(sl);
  }

  D_RETURN_(3);
}

void
alb_document_render_selection_partial(alb_document * doc,
                                      int x,
                                      int y,
                                      int w,
                                      int h)
{
  gib_list *sl, *l;
  int sel_count;

  D_ENTER(3);

  sl = alb_document_get_selected_list(doc);
  D(3, ("selected items count: %d\n", gib_list_length(sl)));

  if (sl) {
    alb_object *obj;

    sel_count = gib_list_length(sl);

    for (l = sl; l; l = l->next) {
      obj = ALB_OBJECT(l->data);
      if (RECTS_INTERSECT
          (x, y, w, h, obj->x - HALF_SEL_WIDTH, obj->y - HALF_SEL_HEIGHT,
           obj->w + (2 * HALF_SEL_WIDTH), obj->h + (2 * HALF_SEL_HEIGHT))) {
        obj->render_selected(obj, doc->im, (sel_count > 1) ? TRUE : FALSE);
      }
    }
    gib_list_free(sl);
  }

  D_RETURN_(3);
}


void
alb_document_render_pmap(alb_document * doc)
{
  D_ENTER(3);
  gib_imlib_render_image_on_drawable(doc->pmap, doc->im, 0, 0, 1, 1, 0);
  D_RETURN_(3);
}

void
alb_document_render_partial(alb_document * document,
                            int x,
                            int y,
                            int w,
                            int h)
{
  gib_list *l;

  D_ENTER(3);

  D(4, ("Doc render partial, %d,%d %dx%d\n", x, y, w, h));
  gib_imlib_image_fill_rectangle(document->im, x, y, w, h, 255, 255, 255,
                                 255);

  alb_fill_render(document->bg_fill, document->im, x, y, w, h);

  for (l = document->layers; l; l = l->next)
    alb_layer_render_partial((alb_layer *) l->data, document->im, x, y, w, h);

  D_RETURN_(3);

}

void
alb_document_render_pmap_partial(alb_document * doc,
                                 int x,
                                 int y,
                                 int w,
                                 int h)
{
  D_ENTER(3);
  gib_imlib_render_image_part_on_drawable_at_size(doc->pmap, doc->im, x, y, w,
                                                  h, x, y, w, h, 1, 1, 0);
  D_RETURN_(3);

}

void
alb_document_add_object(alb_document * doc,
                        alb_object * obj)
{
  gib_list *top;

  D_ENTER(3);

  top = gib_list_last(doc->layers);
  alb_layer_add_object(((alb_layer *) top->data), obj);
  alb_object_dirty(obj);

  D_RETURN_(3);
}

void
alb_document_add_layer(alb_document * doc)
{
  alb_layer *layer;

  D_ENTER(3);

  layer = alb_layer_new();
  layer->doc = doc;
  doc->layers = gib_list_add_end(doc->layers, layer);

  D_RETURN_(3);
}

alb_object *
alb_document_find_clicked_object(alb_document * doc,
                                 int x,
                                 int y)
{
  gib_list *l;
  alb_object *ret = NULL;

  D_ENTER(3);

  for (l = doc->layers; l; l = l->next) {
    if (ALB_LAYER(l->data)->visible)
      ret = alb_layer_find_clicked_object(ALB_LAYER(l->data), x, y);
  }

  D_RETURN(3, ret);

}

void
alb_document_render_full(alb_document * d,
                         int selection)
{
  D_ENTER(3);

  alb_document_render(d);
  if (selection)
    alb_document_render_selection(d);
  alb_document_render_pmap(d);

  alb_document_render_to_window(d);

  D_RETURN_(3);
}

void
alb_document_render_updates(alb_document * d,
                            unsigned char clear)
{
  D_ENTER(3);

  if (d && d->up) {
    int x, y, w, h;
    Imlib_Updates u;

    d->up = imlib_updates_merge_for_rendering(d->up, d->w, d->h);
    for (u = d->up; u; u = imlib_updates_get_next(u)) {
      imlib_updates_get_coordinates(u, &x, &y, &w, &h);
      alb_document_render_partial(d, x, y, w, h);
      alb_document_render_selection_partial(d, x, y, w, h);
      alb_document_render_pmap_partial(d, x, y, w, h);
      if (clear)
        alb_document_render_to_window_partial(d, x, y, w, h);
    }
    imlib_updates_free(d->up);
    d->up = NULL;
  }
  D_RETURN_(3);
}

void
alb_document_remove_object(alb_document * d,
                           alb_object * obj)
{
  D_ENTER(3);
  alb_object_dirty(obj);
  alb_layer_remove_object(obj->layer, obj);

  D_RETURN_(3);
}

void
alb_document_dirty_area(alb_document * doc,
                        int x,
                        int y,
                        int w,
                        int h)
{
  doc->up = imlib_update_append_rect(doc->up, x, y, w, h);
}

void
alb_document_dirty_selection(alb_document * doc)
{
  gib_list *l, *list;

  D_ENTER(3);

  list = alb_document_get_selected_list(doc);

  for (l = list; l; l = l->next)
    alb_object_dirty_selection(ALB_OBJECT(l->data));

  gib_list_free(list);

  D_RETURN_(3);
}

void
alb_document_rename(alb_document * d,
                    char *name)
{
  D_ENTER(3);

  if (d->name)
    efree(d->name);
  d->name = estrdup(name);

  D_RETURN_(3);
}

int
alb_document_save(alb_document * doc,
                  char *filename)
{
  char *ext;

  D_ENTER(3);

  ext = strrchr(filename, '.');
  if (ext)
    ext++;

  if (ext) {
    if (!strcasecmp(ext, "xml"))
      alb_document_save_xml(doc, filename);
    else
      alb_document_save_imlib(doc, filename);
  } else
    alb_document_save_xml(doc, filename);

  D_RETURN(3, 1);
}

/* TODO Error checking here */
int
alb_document_save_imlib(alb_document * doc,
                        char *filename)
{
  D_ENTER(3);

  /* render all but the selection */
  alb_document_render_full(doc, 0);

  gib_imlib_save_image(doc->im, filename);

  D_RETURN(3, 1);
}

void
alb_document_resize(alb_document * doc,
                    int w,
                    int h)
{
  D_ENTER(3);

  if (w == 0)
    w = 1;
  if (h == 0)
    h = 1;

  printf("resizing to %dx%d\n", w, h);
  if ((w != doc->w) || (h != doc->h)) {
    int old_w, old_h;

    old_w = doc->w;
    old_h = doc->h;
    doc->w = w;
    doc->h = h;
    if (doc->pmap)
      XFreePixmap(disp, doc->pmap);
    doc->pmap = XCreatePixmap(disp, root, w, h, depth);
    if (doc->im)
      gib_imlib_free_image(doc->im);
    doc->im = imlib_create_image(w, h);
    alb_document_render_full(doc, FALSE);
#if 1
    if (w > old_w) {
      alb_document_dirty_area(doc, old_w, 0, w - old_w, h);
    }
    if (h > old_h) {
      alb_document_dirty_area(doc, 0, old_h, w, h - old_h);
    }
    alb_document_render_updates(doc, TRUE);
#endif
  }

  D_RETURN_(3);
}

void
alb_document_tesselate(alb_document * doc)
{
  int h;
  gib_list *l, *ll, *lll, *objs, *points;
  alb_object *obj;
  struct point *p;

  for (l = doc->layers; l; l = l->next) {
    int count;

    ll = ALB_LAYER(l->data)->objects;
    count = gib_list_length(ll);
    lll = alb_thumb_tesselate_constrain_w(doc->w, &h, 100, 100, 5, 5, count);
    if (gib_list_length(lll) != count) {
      weprintf("wrong!, gave it %d, got %d back\n", count,
               gib_list_length(lll));
    } else {
      alb_document_resize(doc, doc->w, h);
      points = lll;
      for (objs = ll; objs; objs = objs->next) {
        obj = ALB_OBJECT(ll->data);
        p = (struct point *) points->data;
        alb_object_move(obj, p->x, p->y);
        printf("moving to %d,%d\n", p->x, p->y);
        points = points->next;
        alb_document_render_updates(doc, TRUE);
      }
      gib_list_free_and_data(lll);
    }
  }
}
