/*
 * Create a new rectangle object
 * @param ev The evoak server connection on whhich to create the object
 * @return A new object handle
 * 
 * This function will create a new rectangle object on the server and return a
 * pointer to a local object structure that will be used to address the
 * remotely created object. On failure a NULL pointer is returned.
 */
Evoak_Object *
evoak_object_rectangle_add(Evoak *ev)
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
   o->magic2 = EVOAK_OBJECT_RECTANGLE_MAGIC;
   o->type = "rectangle";
   o->r = 255; o->g = 255; o->b = 255; o->a = 255;
   p.type = EVOAK_OBJECT_TYPE_RECTANGLE;
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
   return o;
}


