static void *
_evoak_object_line_meta_new(Evoak_Object *o)
{
   Evoak_Meta_Line *m;

   m = calloc(1, sizeof(Evoak_Meta_Line));
   if (!m) return NULL;
   return m;
}

static void
_evoak_object_line_meta_free(void *meta)
{
   Evoak_Meta_Line *m;

   m = meta;
   free(m);
}

/*
 * Create a new line object
 * @param ev The evoak server connection on whhich to create the object
 * @return A new object handle
 * 
 * This function will create a new line object on the server and return a
 * pointer to a local object structure that will be used to address the
 * remotely created object. On failure a NULL pointer is returned.
 */
Evoak_Object *
evoak_object_line_add(Evoak *ev)
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
   o->magic2 = EVOAK_OBJECT_LINE_MAGIC;
   o->type = "line";
   o->r = 255; o->g = 255; o->b = 255; o->a = 255;
   p.type = EVOAK_OBJECT_TYPE_LINE;
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
   o->meta = _evoak_object_line_meta_new(o);
   o->meta_free = _evoak_object_line_meta_free;
   return o;
}

void
evoak_object_line_xy_set(Evoak_Object *o, Evoak_Coord x1, Evoak_Coord y1, Evoak_
Coord x2, Evoak_Coord y2)
{
   unsigned char *d;
   int s;
   Evoak_PR_Object_Line_Set p;

   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->magic2 != EVOAK_OBJECT_LINE_MAGIC) return;
   if (o->delete_me) return;
     {
        Evoak_Meta_Line *m;

        m = o->meta;
        if ((x1 == m->x1) && (y1 == m->y1) &&
            (x2 == m->x2) && (y2 == m->y2)) return;
     }
   p.x1 = x1;
   p.y1 = y1;
   p.x2 = x2;
   p.y2 = y2;
   if (x1 <= x2)
     {
        o->x = x1;
        o->w = (x2 - x1) + 1;
     }
   else
     {
        o->x = x2;
        o->w = (x1 - x2) + 1;
     }
   if (y1 <= y2)
     {
        o->y = y1;
        o->h = (y2 - y1) + 1;
     }
   else
     {
        o->y = y2;
        o->h = (y1 - y2) + 1;
     }
   d = _evoak_proto[EVOAK_PR_OBJECT_LINE_SET].enc(&p, &s);
   if (d)
     {
        _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_LINE_SET, o->id, 0, d, s);
        free(d);
     }
}

void
evoak_object_line_xy_get(Evoak_Object *o, Evoak_Coord *x1, Evoak_Coord *y1, Evoa
k_Coord *x2, Evoak_Coord *y2)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->magic2 != EVOAK_OBJECT_LINE_MAGIC) return;
   if (o->delete_me) return;
     {
        Evoak_Meta_Line *m;

        m = o->meta;
        if (x1) *x1 = m->x1;
        if (y1) *y1 = m->y1;
        if (x2) *x2 = m->x2;
        if (y2) *y2 = m->y2;
     }
}
