static void *
_evoak_object_polygon_meta_new(Evoak_Object *o)
{
   Evoak_Meta_Polygon *m;

   m = calloc(1, sizeof(Evoak_Meta_Polygon));
   if (!m) return NULL;
   return m;
}

static void
_evoak_object_polygon_meta_free(void *meta)
{
   Evoak_Meta_Polygon *m;

   m = meta;
   while (m->points)
     {
        free(m->points->data);
        m->points = evas_list_remove(m->points, m->points->data);
     }
   free(m);
}

/*
 * Create a new polygon object
 * @param ev The evoak server connection on whhich to create the object
 * @return A new object handle
 * 
 * This function will create a new polygon object on the server and return a
 * pointer to a local object structure that will be used to address the
 * remotely created object. On failure a NULL pointer is returned.
 */
Evoak_Object *
evoak_object_polygon_add(Evoak *ev)
{
   unsigned char *d;
   int s;
   Evoak_PR_Object_Add p;
   Evoak_Object *o;

   if ((!ev) || (ev->magic != EVOAK_MAGIC)) return NULL;
   o = calloc(1, sizeof(Evoak_Object));
   if (!o) return NULL;
   o->evoak = ev;
   o->magic = EVOAK_OBJECT_MAGIC;
   o->magic2 = EVOAK_OBJECT_POLYGON_MAGIC;
   o->type = "polygon";
   o->r = 255; o->g = 255; o->b = 255; o->a = 255;
   p.type = EVOAK_OBJECT_TYPE_POLYGON;
   o->id = ev->object_id;
   _evoak_object_id_inc(ev);
   d = _evoak_proto[EVOAK_PR_OBJECT_ADD].enc(&p, &s);
   if (d)
     {
        _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_ADD, o->id, 0, d, s);
        free(d);
     }
   _evoak_hash_store(&(ev->object_hash), o->id, o);
   _evoak_object_layer_add(o);
   _evoak_object_ref(o);
   o->meta = _evoak_object_polygon_meta_new(o);
   o->meta_free = _evoak_object_polygon_meta_free;
   return o;
}

void
evoak_object_polygon_point_add(Evoak_Object *o, Evoak_Coord x, Evoak_Coord y)
{
   unsigned char *d;
   int s;
   Evoak_PR_Object_Poly_Point_Add p;

   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->magic2 != EVOAK_OBJECT_POLYGON_MAGIC) return;
   if (o->delete_me) return;
     {
        Evoak_Meta_Polygon *m;

        m = o->meta;
        if (!m->points)
          {
             o->x = x;
             o->y = y;
             o->w = 1;
             o->h = 1;
          }
          {
             Evoak_Meta_Polygon_Point *pt;

             pt = calloc(1, sizeof(Evoak_Meta_Polygon_Point));
             if (!pt) return;
             pt->x = x;
             pt->y = y;
             m->points = evas_list_append(m->points, pt);
             if (x < o->x)
               {
                  o->w += (o->x - x);
                  o->x = x;
               }
             else if (x >= (o->x + o->w))
               {
                  o->w = x - o->x + 1;
               }
             if (y < o->y)
               {
                  o->h += (o->y - y);
                  o->y = y;
               }
             else if (y >= (o->y + o->h))
               {
                  o->h = y - o->y + 1;
               }
          }
     }
   p.x = x;
   p.y = y;
   d = _evoak_proto[EVOAK_PR_OBJECT_POLY_POINT_ADD].enc(&p, &s);
   if (d)
     {
        _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_POLY_POINT_ADD, o->id, 0, d, s
);
        free(d);
     }
}

void
evoak_object_polygon_points_clear(Evoak_Object *o)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->magic2 != EVOAK_OBJECT_POLYGON_MAGIC) return;
   if (o->delete_me) return;
     {
        Evoak_Meta_Polygon *m;

        m = o->meta;
        if (!m->points) return;
        while (m->points)
          {
             free(m->points->data);
             m->points = evas_list_remove(m->points, m->points->data);
          }
        o->x = 0;
        o->y = 0;
        o->w = 0;
        o->h = 0;
     }
   _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_POLY_POINT_CLEAR, o->id, 0, NULL, 0
);
}


