/*
 * Delete an object from the canvas
 * @param o The object to delete
 * 
 * This function will delete the specified object @p o from the evoak canvas
 * it belongs to. The client will recieve no further events for this object
 * after this call is complete.
 */
void
evoak_object_del(Evoak_Object *o)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->delete_me) return;
   _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_DEL, o->id, 0, NULL, 0);
   _evoak_object_ref(o);
   _evoak_callback_call(o, EVOAK_CALLBACK_FREE, NULL);
   _evoak_object_unref(o);
   o->delete_me = 1;
   if (o->swallow)
     {
        o->swallow->swallowees = evas_list_remove(o->swallow->swallowees, o);
        o->swallow = NULL;
     }
   while (o->swallowees)
     {
        Evoak_Object *o2;

        o2 = o->swallowees->data;
        o2->swallow = NULL;
        o->swallowees = evas_list_remove_list(o->swallowees, o->swallowees);
     }
   _evoak_object_unref(o);
}

const char *
evoak_object_type_get(Evoak_Object *o)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return "";
   if (o->delete_me) return "";
   return o->type;
}

void
evoak_object_move(Evoak_Object *o, Evoak_Coord x, Evoak_Coord y)
{
   unsigned char *d;
   int s;
   int dx, dy;

   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->delete_me) return;
   if ((x == o->x) && (y == o->y)) return;
   /* FIXME: adjust coords for line & poly objs */

   if (((-128 <= x) && (x < 128)) && ((-128 <= y) && (y < 128)))
     {
        Evoak_PR_Object_Move8 p;

        p.x = x;
        p.y = y;
        d = _evoak_proto[EVOAK_PR_OBJECT_MOVE8].enc(&p, &s);
        if (d)
          {
             _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_MOVE8, o->id, 0, d, s);
             free(d);
          }
        o->x = x;
        o->y = y;
        return;
     }
   dx = x - o->x;
   dy = y - o->y;
   if (((-128 <= dx) && (dx < 128)) && ((-128 <= dy) && (dy < 128)))
     {
        Evoak_PR_Object_Move_Rel8 p;

        p.x = dx;
        p.y = dy;
        d = _evoak_proto[EVOAK_PR_OBJECT_MOVE_REL8].enc(&p, &s);
        if (d)
          {
             _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_MOVE_REL8, o->id, 0, d, s
);
             free(d);
          }
        o->x = x;
        o->y = y;
        return;

     }
   if (((-32768 <= x) && (x < 32768)) && ((-32768 <= y) && (y < 32768)))
     {
        Evoak_PR_Object_Move16 p;

        p.x = x;
        p.y = y;
        d = _evoak_proto[EVOAK_PR_OBJECT_MOVE16].enc(&p, &s);
        if (d)
          {
             _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_MOVE16, o->id, 0, d, s);
             free(d);
          }
        o->x = x;
        o->y = y;
        return;
     }
   if (((-32768 <= dx) && (dx < 32768)) && ((-32768 <= dy) && (dy < 32768)))
     {
        Evoak_PR_Object_Move_Rel16 p;

        p.x = dx;
        p.y = dy;
        d = _evoak_proto[EVOAK_PR_OBJECT_MOVE_REL16].enc(&p, &s);
        if (d)
          {
             _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_MOVE_REL16, o->id, 0, d,
s);
             free(d);
          }
        o->x = x;
        o->y = y;
        return;
     }

     {
        Evoak_PR_Object_Move p;

        p.x = x;
        p.y = y;
        d = _evoak_proto[EVOAK_PR_OBJECT_MOVE].enc(&p, &s);
        if (d)
          {
             _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_MOVE, o->id, 0, d, s);
             free(d);
          }
        o->x = x;
        o->y = y;
     }
}

void
evoak_object_resize(Evoak_Object *o, Evoak_Coord w, Evoak_Coord h)
{
   unsigned char *d;
   int s;
   int dh, dw;

   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->delete_me) return;
   if (w < 0) w = 0;
   if (h < 0) h = 0;
   if ((w == o->w) && (h == o->h)) return;
   /* FIXME: disallow for text objects */
   /* FIXME: adjust coords for line & poly objs */

   if (((0 <= w) && (w < 256)) && ((0 <= h) && (h < 256)))
     {
        Evoak_PR_Object_Resize8 p;

        p.w = w;
        p.h = h;
        d = _evoak_proto[EVOAK_PR_OBJECT_RESIZE8].enc(&p, &s);
        if (d)
          {
             _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_RESIZE8, o->id, 0, d, s);
             free(d);
          }
        o->w = w;
        o->h = h;
        return;
     }
   dw = w - o->w;
   dh = h - o->h;
   if (((-128 <= dw) && (dw < 128)) && ((-128 <= dh) && (dh < 128)))
     {
        Evoak_PR_Object_Resize_Rel8 p;

        p.w = dw;
        p.h = dh;
        d = _evoak_proto[EVOAK_PR_OBJECT_RESIZE_REL8].enc(&p, &s);
        if (d)
          {
             _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_RESIZE_REL8, o->id, 0, d,
 s);
             free(d);
          }
        o->w = w;
        o->h = h;
        return;
     }
   if (((-32768 <= w) && (w < 32768)) && ((-32768 <= h) && (h < 32768)))
     {
        Evoak_PR_Object_Resize16 p;

        p.w = w;
        p.h = h;
        d = _evoak_proto[EVOAK_PR_OBJECT_RESIZE16].enc(&p, &s);
        if (d)
          {
             _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_RESIZE16, o->id, 0, d, s)
;
             free(d);
          }
        o->w = w;
        o->h = h;
        return;
     }
   if (((-32768 <= dw) && (dw < 32768)) && ((-32768 <= dh) && (dh < 32768)))
     {
        Evoak_PR_Object_Resize_Rel16 p;

        p.w = dw;
        p.h = dh;
        d = _evoak_proto[EVOAK_PR_OBJECT_RESIZE_REL16].enc(&p, &s);
        if (d)
          {
             _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_RESIZE_REL16, o->id, 0, d
, s);
             free(d);
          }
        o->w = w;
        o->h = h;
        return;
     }

     {
        Evoak_PR_Object_Resize p;

        p.w = w;
        p.h = h;
        d = _evoak_proto[EVOAK_PR_OBJECT_RESIZE].enc(&p, &s);
        if (d)
          {
             _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_RESIZE, o->id, 0, d, s);
             free(d);
          }
        o->w = w;
        o->h = h;
     }
}

void
evoak_object_geometry_get(Evoak_Object *o, Evoak_Coord *x, Evoak_Coord *y, Evoak
_Coord *w, Evoak_Coord *h)
{
   if (o->delete_me) return;
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (x) *x = o->x;
   if (y) *y = o->y;
   if (w) *w = o->w;
   if (h) *h = o->h;
}

void
evoak_object_show(Evoak_Object *o)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->delete_me) return;
   if (o->visible) return;
   o->visible = 1;
   _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_SHOW, o->id, 0, NULL, 0);
}

void
evoak_object_hide(Evoak_Object *o)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->delete_me) return;
   if (!o->visible) return;
   o->visible = 0;
   _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_HIDE, o->id, 0, NULL, 0);
}

Evoak_Bool
evoak_object_visible_get(Evoak_Object *o)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return 0;
   if (o->delete_me) return 0;
   return o->visible;
}

void
evoak_object_color_set(Evoak_Object *o, int r, int g, int b, int a)
{
   unsigned char *d;
   int s;
   Evoak_PR_Object_Color_Set p;

   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->delete_me) return;
   if (r < 0) r = 0; if (r > 255) r = 255;
   if (g < 0) g = 0; if (g > 255) r = 255;
   if (b < 0) b = 0; if (b > 255) r = 255;
   if (a < 0) a = 0; if (a > 255) r = 255;
   if ((o->r == r) && (o->g == g) && (o->b == b) && (o->a == a)) return;
   o->r = r;
   o->g = g;
   o->b = b;
   o->a = a;
   p.r = r;
   p.g = g;
   p.b = b;
   p.a = a;
   d = _evoak_proto[EVOAK_PR_OBJECT_COLOR_SET].enc(&p, &s);
   if (d)
     {
        _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_COLOR_SET, o->id, 0, d, s);
        free(d);
     }
}

void
evoak_object_color_get(Evoak_Object *o, int *r, int *g, int *b, int *a)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->delete_me) return;
   if (r) *r = o->r;
   if (g) *g = o->g;
   if (b) *b = o->b;
   if (a) *a = o->a;
}

Evoak *
evoak_object_evoak_get(Evoak_Object *o)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return NULL;
   if (o->delete_me) return NULL;
   return o->evoak;
}
