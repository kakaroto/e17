static void *
_evoak_object_image_meta_new(Evoak_Object *o)
{
   Evoak_Meta_Image *m;

   m = calloc(1, sizeof(Evoak_Meta_Image));
   if (!m) return NULL;
   m->smooth_scale = 1;
   m->has_alpha = 1;
   return m;
}

static void
_evoak_object_image_meta_free(void *meta)
{
   Evoak_Meta_Image *m;

   m = meta;
   if (m->file) free(m->file);
   if (m->key) free(m->key);
   free(m);
}

/*
 * Create a new image object
 * @param ev The evoak server connection on whhich to create the object
 * @return A new object handle
 * 
 * This function will create a new image object on the server and return a
 * pointer to a local object structure that will be used to address the
 * remotely created object. On failure a NULL pointer is returned.
 */
Evoak_Object *
evoak_object_image_add(Evoak *ev)
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
   o->magic2 = EVOAK_OBJECT_IMAGE_MAGIC;
   o->type = "image";
   o->r = 255; o->g = 255; o->b = 255; o->a = 255;
   p.type = EVOAK_OBJECT_TYPE_IMAGE;
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
   o->meta = _evoak_object_image_meta_new(o);
   o->meta_free = _evoak_object_image_meta_free;
   return o;
}

void
evoak_object_image_file_set(Evoak_Object *o, const char *file, const char *key)
{
   unsigned char *d;
   int s;
   Evoak_PR_Object_Image_File_Set p;
   char buf[4096];

   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->magic2 != EVOAK_OBJECT_IMAGE_MAGIC) return;
   if (o->delete_me) return;
   if (!file) file = "";
   if (!key) key = "";
   if (file[0] != '/')
     {
        static char wd[4096] ="";

        if (wd[0] == 0) getcwd(wd, 4096);
        snprintf(buf, sizeof(buf), "%s/%s", wd, file);
        file = buf;
     }
     {
        Evoak_Meta_Image *m;
        Evas_Imaging_Image *im;

        m = o->meta;
        if ((m->file) && (!strcmp(m->file, file)))
          {
             if ((m->key) && (!strcmp(m->key, key))) return;
          }
        if (m->file) free(m->file);
        if (m->key) free(m->key);
        m->file = strdup(file);
        m->key = strdup(key);
        im = evas_imaging_image_load((char *)file, (char *)key);
        if (im)
          {
             int w, h;

             evas_imaging_image_size_get(im, &w, &h);
             m->image.w = w;
             m->image.h = h;
             m->has_alpha = evas_imaging_image_alpha_get(im);
             evas_imaging_image_free(im);
          }
     }
   p.file = (char *)file;
   p.key = (char *)key;
   d = _evoak_proto[EVOAK_PR_OBJECT_IMAGE_FILE_SET].enc(&p, &s);
   if (d)
     {
        _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_IMAGE_FILE_SET, o->id, 0, d, s
);
        free(d);
     }
}

void
evoak_object_image_file_get(Evoak_Object *o, const char **file, const char **key
)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->magic2 != EVOAK_OBJECT_IMAGE_MAGIC) return;
   if (o->delete_me) return;
     {
        Evoak_Meta_Image *m;

        m = o->meta;
        if (file) *file = m->file;
        if (key) *key = m->key;
     }
}

void
evoak_object_image_fill_set(Evoak_Object *o, Evoak_Coord x, Evoak_Coord y, Evoak
_Coord w, Evoak_Coord h)
{
   unsigned char *d;
   int s;

   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->magic2 != EVOAK_OBJECT_IMAGE_MAGIC) return;
   if (o->delete_me) return;
   if (w < 1) w = 1;
   if (h < 1) h = 1;
     {
        Evoak_Meta_Image *m;

        m = o->meta;

        if ((m->fill.x == x) && (m->fill.y == y) &&
            (m->fill.w == w) && (m->fill.h == h)) return;
        m->fill.x = x;
        m->fill.y = y;
        m->fill.w = w;
        m->fill.h = h;
     }
   if ((x == 0) && (y == 0) && (w == o->w) && (h == o->h))
     {
        _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_IMAGE_FILL_ALL_SET, o->id, 0,
NULL, 0);
        return;
     }
   if ((x == 0) && (y == 0) &&
       ((0 <= w) && (w < 256)) && ((0 <= h) && (h < 256)))
     {
        Evoak_PR_Object_Image_Fill_Size8_Set p;

        p.w = w;
        p.h = h;
        d = _evoak_proto[EVOAK_PR_OBJECT_IMAGE_FILL_SIZE8_SET].enc(&p, &s);
        if (d)
          {
             _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_IMAGE_FILL_SIZE8_SET, o->
id, 0, d, s);
             free(d);
          }
        return;
     }
   if ((x == 0) && (y == 0) &&
       ((0 <= w) && (w < 32768)) && ((0 <= h) && (h < 32768)))
     {
        Evoak_PR_Object_Image_Fill_Size16_Set p;

        p.w = w;
        p.h = h;
        d = _evoak_proto[EVOAK_PR_OBJECT_IMAGE_FILL_SIZE16_SET].enc(&p, &s);
        if (d)
          {
             _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_IMAGE_FILL_SIZE16_SET, o-
>id, 0, d, s);
             free(d);
          }
        return;
     }
   if (((-128 <= x) && (x < 128)) && ((-128 <= y) && (y < 128)) &&
       ((0 <= w) && (w < 256)) && ((0 <= h) && (h < 256)))
     {
        Evoak_PR_Object_Image_Fill8_Set p;

        p.x = x;
        p.y = y;
        p.w = w;
        p.h = h;
        d = _evoak_proto[EVOAK_PR_OBJECT_IMAGE_FILL8_SET].enc(&p, &s);
        if (d)
          {
             _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_IMAGE_FILL8_SET, o->id, 0
, d, s);
             free(d);
          }
        return;
     }
   if (((-32768 <= x) && (x < 32768)) && ((-32768 <= y) && (y < 32768)) &&
       ((0 <= w) && (w < 32768)) && ((0 <= h) && (h < 32768)))
     {
        Evoak_PR_Object_Image_Fill16_Set p;

        p.x = x;
        p.y = y;
        p.w = w;
        p.h = h;
        d = _evoak_proto[EVOAK_PR_OBJECT_IMAGE_FILL16_SET].enc(&p, &s);
        if (d)
          {
             _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_IMAGE_FILL16_SET, o->id,
0, d, s);
             free(d);
          }
        return;
     }
     {
        Evoak_PR_Object_Image_Fill_Set p;

        p.x = x;
        p.y = y;
        p.w = w;
        p.h = h;
        d = _evoak_proto[EVOAK_PR_OBJECT_IMAGE_FILL_SET].enc(&p, &s);
        if (d)
          {
             _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_IMAGE_FILL_SET, o->id, 0,
 d, s);
             free(d);
          }
     }
}

void
evoak_object_image_fill_get(Evoak_Object *o, Evoak_Coord *x, Evoak_Coord *y, Evo
ak_Coord *w, Evoak_Coord *h)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->magic2 != EVOAK_OBJECT_IMAGE_MAGIC) return;
   if (o->delete_me) return;
     {
        Evoak_Meta_Image *m;

        m = o->meta;
        if (x) *x = m->fill.x;
        if (y) *y = m->fill.y;
        if (w) *w = m->fill.w;
        if (h) *h = m->fill.h;
     }
}

void
evoak_object_image_border_set(Evoak_Object *o, int l, int r, int t, int b)
{
   unsigned char *d;
   int s;
   Evoak_PR_Object_Image_Border_Set p;

   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->magic2 != EVOAK_OBJECT_IMAGE_MAGIC) return;
   if (o->delete_me) return;
   if (l < 0) l = 0;
   if (r < 0) r = 0;
   if (t < 0) t = 0;
   if (b < 0) b = 0;
     {
        Evoak_Meta_Image *m;

        m = o->meta;

        if ((m->border.l == l) && (m->border.r == r) &&
            (m->border.t == t) && (m->border.b == b)) return;
        m->border.l = l;
        m->border.r = r;
        m->border.t = t;
        m->border.b = b;
     }
   p.l = l;
   p.r = r;
   p.t = t;
   p.b = b;
   d = _evoak_proto[EVOAK_PR_OBJECT_IMAGE_BORDER_SET].enc(&p, &s);
   if (d)
     {
        _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_IMAGE_BORDER_SET, o->id, 0, d,
 s);
        free(d);
     }
}

void
evoak_object_image_border_get(Evoak_Object *o, int *l, int *r, int *t, int *b)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->magic2 != EVOAK_OBJECT_IMAGE_MAGIC) return;
   if (o->delete_me) return;
     {
        Evoak_Meta_Image *m;

        m = o->meta;
        if (l) *l = m->border.l;
        if (r) *r = m->border.r;
        if (t) *t = m->border.t;
        if (b) *b = m->border.b;
     }
}

void
evoak_object_image_smooth_scale_set(Evoak_Object *o, Evoak_Bool onoff)
{
   unsigned char *d;
   int s;
   Evoak_PR_Object_Image_Smooth_Scale_Set p;

   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->magic2 != EVOAK_OBJECT_IMAGE_MAGIC) return;
   if (o->delete_me) return;
     {
        Evoak_Meta_Image *m;

        m = o->meta;

        if (((m->smooth_scale) && (onoff)) ||
            ((!m->smooth_scale) && (!onoff))) return;
        m->smooth_scale = onoff;
     }
   p.onoff = onoff;
   d = _evoak_proto[EVOAK_PR_OBJECT_IMAGE_SMOOTH_SCALE_SET].enc(&p, &s);
   if (d)
     {
        _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_IMAGE_SMOOTH_SCALE_SET, o->id,
 0, d, s);
        free(d);
     }
}

Evoak_Bool
evoak_object_image_smooth_scale_get(Evoak_Object *o)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return 0;
   if (o->magic2 != EVOAK_OBJECT_IMAGE_MAGIC) return 0;
   if (o->delete_me) return 0;
     {
        Evoak_Meta_Image *m;

        m = o->meta;
        return m->smooth_scale;
     }
   return 0;
}

void
evoak_object_image_size_set(Evoak_Object *o, int w, int h)
{
   unsigned char *d;
   int s;
   Evoak_PR_Object_Image_Size_Set p;

   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->magic2 != EVOAK_OBJECT_IMAGE_MAGIC) return;
   if (w < 0) w = 0;
   if (h < 0) h = 0;
   if (o->delete_me) return;
     {
        Evoak_Meta_Image *m;

        m = o->meta;

        if ((m->image.w == w) && (m->image.h == h)) return;
        m->image.w = w;
        m->image.h = h;
     }
   p.w = w;
   p.h = h;
   d = _evoak_proto[EVOAK_PR_OBJECT_IMAGE_SIZE_SET].enc(&p, &s);
   if (d)
     {
        _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_IMAGE_SIZE_SET, o->id, 0, d, s
);
        free(d);
     }
}

void
evoak_object_image_size_get(Evoak_Object *o, int *w, int *h)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->magic2 != EVOAK_OBJECT_IMAGE_MAGIC) return;
   if (o->delete_me) return;
     {
        Evoak_Meta_Image *m;

        m = o->meta;
        if (w) *w = m->image.w;
        if (h) *h = m->image.h;
     }
}

void
evoak_object_image_alpha_set(Evoak_Object *o, Evoak_Bool onoff)
{
   unsigned char *d;
   int s;
   Evoak_PR_Object_Image_Alpha_Set p;

   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->magic2 != EVOAK_OBJECT_IMAGE_MAGIC) return;
   if (o->delete_me) return;
     {
        Evoak_Meta_Image *m;

        m = o->meta;

        if (((m->has_alpha) && (onoff)) ||
            ((!m->has_alpha) && (!onoff))) return;
        m->has_alpha = onoff;
     }
   p.onoff = onoff;
   d = _evoak_proto[EVOAK_PR_OBJECT_IMAGE_ALPHA_SET].enc(&p, &s);
   if (d)
     {
        _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_IMAGE_ALPHA_SET, o->id, 0, d,
s);
        free(d);
     }
}

Evoak_Bool
evoak_object_image_alpha_get(Evoak_Object *o)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return 0;
   if (o->magic2 != EVOAK_OBJECT_IMAGE_MAGIC) return 0;
   if (o->delete_me) return 0;
     {
        Evoak_Meta_Image *m;

        m = o->meta;
        return m->has_alpha;
     }
}
