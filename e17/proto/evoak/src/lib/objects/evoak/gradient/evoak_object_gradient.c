static void *
_evoak_object_gradient_meta_new(Evoak_Object *o)
{
   Evoak_Meta_Gradient *m;

   m = calloc(1, sizeof(Evoak_Meta_Gradient));
   if (!m) return NULL;
   return m;
}

static void
_evoak_object_gradient_meta_free(void *meta)
{
   Evoak_Meta_Gradient *m;

   m = meta;
   free(m);
}

/*
 * Create a new gradient object
 * @param ev The evoak server connection on whhich to create the object
 * @return A new object handle
 * 
 * This function will create a new gradient object on the server and return a
 * pointer to a local object structure that will be used to address the
 * remotely created object. On failure a NULL pointer is returned.
 */
Evoak_Object *
evoak_object_gradient_add(Evoak *ev)
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
   o->magic2 = EVOAK_OBJECT_GRADIENT_MAGIC;
   o->type = "gradient";
   o->r = 255; o->g = 255; o->b = 255; o->a = 255;
   p.type = EVOAK_OBJECT_TYPE_GRADIENT;
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
   o->meta = _evoak_object_gradient_meta_new(o);
   o->meta_free = _evoak_object_gradient_meta_free;
   return o;
}

void
evoak_object_gradient_color_add(Evoak_Object *o, int r, int g, int b, int a, int
 distance)
{
   unsigned char *d;
   int s;
   Evoak_PR_Object_Grad_Color_Add p;

   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->magic2 != EVOAK_OBJECT_GRADIENT_MAGIC) return;
   if (o->delete_me) return;
   p.r = r;
   p.g = g;
   p.b = b;
   p.a = a;
   p.dist = distance;
   d = _evoak_proto[EVOAK_PR_OBJECT_GRAD_COLOR_ADD].enc(&p, &s);
   if (d)
     {
        _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_GRAD_COLOR_ADD, o->id, 0, d, s
);
        free(d);
    }
}

void
evoak_object_gradient_colors_clear(Evoak_Object *o)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->magic2 != EVOAK_OBJECT_GRADIENT_MAGIC) return;
   if (o->delete_me) return;
   _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_GRAD_COLOR_CLEAR, o->id, 0, NULL, 0
);
}

void
evoak_object_gradient_angle_set(Evoak_Object *o, Evoak_Angle angle)
{
   unsigned char *d;
   int s;
   Evoak_PR_Object_Grad_Angle_Set p;

   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->magic2 != EVOAK_OBJECT_GRADIENT_MAGIC) return;
   if (o->delete_me) return;
     {
        Evoak_Meta_Gradient *m;

        m = o->meta;
        if (m->angle == angle) return;
        m->angle = angle;
     }
   p.ang100 = angle * 100;
   d = _evoak_proto[EVOAK_PR_OBJECT_GRAD_ANGLE_SET].enc(&p, &s);
   if (d)
     {
        _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_GRAD_ANGLE_SET, o->id, 0, d, s
);
        free(d);
    }
}

Evoak_Angle
evoak_object_gradient_angle_get(Evoak_Object *o)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return 0;
   if (o->magic2 != EVOAK_OBJECT_GRADIENT_MAGIC) return 0;
   if (o->delete_me) return 0;
     {
        Evoak_Meta_Gradient *m;

        m = o->meta;
        return m->angle;
     }
   return 0;
}

