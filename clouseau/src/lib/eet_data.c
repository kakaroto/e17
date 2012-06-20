#include "eet_data.h"
static data_desc *desc = NULL;

void
lines_free(bmp_info_st *st)
{  /* Free lines asociated with a bmp */
   if (st->lx)
     evas_object_del(st->lx);

   if (st->ly)
     evas_object_del(st->ly);

   st->lx = st->ly = NULL;
}

void
bmp_blob_free(bmp_info_st *st)
{  /* We also free all lines drawn in this bmp canvas */
   lines_free(st);

   if (st->bmp)
     free(st->bmp);
}

void
item_tree_item_free(Tree_Item *parent)
{
   Tree_Item *treeit;

   EINA_LIST_FREE(parent->children, treeit)
     {
        item_tree_item_free(treeit);
     }

   obj_information_free(parent->info);
   eina_stringshare_del(parent->name);
   free(parent);
}

void
item_tree_free(Eina_List *tree)
{
   Tree_Item *treeit;

   EINA_LIST_FREE(tree, treeit)
     {
        item_tree_item_free(treeit);
     }
}

void
_item_tree_item_string(Tree_Item *parent)
{  /* Use this function if you want to print tree for debug */
   Tree_Item *treeit;
   Eina_List *l;

   EINA_LIST_FOREACH(parent->children, l, treeit)
     {
        _item_tree_item_string(treeit);
     }

   if (parent->name)
     printf("<%s>\n", parent->name);
}

static eet_message_type_mapping eet_mapping[] = {
       { GUI_CLIENT_CONNECT, GUI_CONNECT_STR },
       { APP_CLIENT_CONNECT, APP_CONNECT_STR },
       { APP_ADD, APP_ADD_STR },
       { DATA_REQ, DATA_REQ_STR },
       { TREE_DATA, TREE_DATA_STR },
       { APP_CLOSED, APP_CLOSED_STR },
       { HIGHLIGHT, HIGHLIGHT_STR },
       { BMP_REQ, BMP_REQ_STR },
       { BMP_DATA, BMP_DATA_STR },
       { UNKNOWN, NULL }
};

message_type
packet_mapping_type_get(const char *name)
{
   int i;
   if (!name)
     return UNKNOWN;

   for (i = 0; eet_mapping[i].name != NULL; ++i)
     if (strcmp(name, eet_mapping[i].name) == 0)
       return eet_mapping[i].t;

   return UNKNOWN;
}

const char *
packet_mapping_type_str_get(message_type t)
{
   int i;
   for (i = 0; eet_mapping[i].name != NULL; ++i)
     if (t == eet_mapping[i].t)
       return eet_mapping[i].name;

   return NULL;
}

const char *
_variant_type_get(const void *data, Eina_Bool  *unknow)
{
   const Variant_Type_st *type = data;
   int i;

   if (unknow)
     *unknow = type->unknow;

   for (i = 0; eet_mapping[i].name != NULL; ++i)
     if (strcmp(type->type, eet_mapping[i].name) == 0)
       return eet_mapping[i].name;

   if (unknow)
     *unknow = EINA_FALSE;

   return type->type;
} /* _variant_type_get */

Eina_Bool
_variant_type_set(const char *type,
      void       *data,
      Eina_Bool   unknow)
{
   Variant_Type_st *vt = data;

   vt->type = type;
   vt->unknow = unknow;
   return EINA_TRUE;
} /* _variant_type_set */

void
variant_free(Variant_st *v)
{
   if (v->data)
     free(v->data);

   free(v);
}

Variant_st *
variant_alloc(message_type t, size_t size, void *info)
{
   if (t != UNKNOWN)
     { /* This will allocate variant and message struct */
        Variant_st *v =  malloc(sizeof(Variant_st));
        v->data = malloc(size);
        _variant_type_set(packet_mapping_type_str_get(t), &v->t, EINA_FALSE);
        memcpy(v->data, info, size);

        return v;
     }

   return NULL;
}

Eet_Data_Descriptor *
connect_desc_make(void)
{
   Eet_Data_Descriptor *d;

   Eet_Data_Descriptor_Class eddc;
   EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, connect_st);
   d = eet_data_descriptor_stream_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_BASIC (d, connect_st, "pid", pid, EET_T_UINT);
   EET_DATA_DESCRIPTOR_ADD_BASIC (d, connect_st, "name", name, EET_T_STRING);

   return d;
}

Eet_Data_Descriptor *
app_add_desc_make(void)
{  /* view field not transferred, will be loaded on user request */
   Eet_Data_Descriptor *d;

   Eet_Data_Descriptor_Class eddc;
   EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, app_info_st);
   d = eet_data_descriptor_stream_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_BASIC (d, app_info_st, "pid", pid, EET_T_UINT);
   EET_DATA_DESCRIPTOR_ADD_BASIC (d, app_info_st, "name", name, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC (d, app_info_st, "ptr", ptr, EET_T_ULONG_LONG);

   return d;
}

Eet_Data_Descriptor *
data_req_desc_make(void)
{
   Eet_Data_Descriptor *d;

   Eet_Data_Descriptor_Class eddc;
   EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, data_req_st);
   d = eet_data_descriptor_stream_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_BASIC (d, data_req_st, "gui", gui, EET_T_ULONG_LONG);
   EET_DATA_DESCRIPTOR_ADD_BASIC (d, data_req_st, "app", app, EET_T_ULONG_LONG);

   return d;
}

Eet_Data_Descriptor *
bmp_info_desc_make(void)
{
   Eet_Data_Descriptor *d;

   Eet_Data_Descriptor_Class eddc;
   EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, bmp_info_st);
   d = eet_data_descriptor_stream_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_BASIC (d, bmp_info_st,
         "gui", gui, EET_T_ULONG_LONG);
   EET_DATA_DESCRIPTOR_ADD_BASIC (d, bmp_info_st,
         "app", app, EET_T_ULONG_LONG);
   EET_DATA_DESCRIPTOR_ADD_BASIC (d, bmp_info_st,
         "object", object, EET_T_ULONG_LONG);
   EET_DATA_DESCRIPTOR_ADD_BASIC (d, bmp_info_st, "ctr",
         ctr, EET_T_ULONG_LONG);
   EET_DATA_DESCRIPTOR_ADD_BASIC (d, bmp_info_st, "w",
         w, EET_T_ULONG_LONG);
   EET_DATA_DESCRIPTOR_ADD_BASIC (d, bmp_info_st, "h",
         h, EET_T_ULONG_LONG);

   return d;
}

Eet_Data_Descriptor *
shot_list_desc_make(void)
{
   Eet_Data_Descriptor *d;

   Eet_Data_Descriptor_Class eddc;
   EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, shot_list_st);
   d = eet_data_descriptor_stream_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_LIST  (d, shot_list_st,
         "view", view, desc->bmp_info);  /* Carefull - init this first */

   return d;
}

Eet_Data_Descriptor *
tree_data_desc_make(void)
{
   Eet_Data_Descriptor *d;

   Eet_Data_Descriptor_Class eddc;
   EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, tree_data_st);
   d = eet_data_descriptor_stream_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_BASIC (d, tree_data_st,
         "gui", gui, EET_T_ULONG_LONG);
   EET_DATA_DESCRIPTOR_ADD_BASIC (d, tree_data_st,
         "app", app, EET_T_ULONG_LONG);
   EET_DATA_DESCRIPTOR_ADD_LIST  (d, tree_data_st,
         "tree", tree, desc->tree);  /* Carefull - init this first */

   return d;
}

Eet_Data_Descriptor *
app_closed_desc_make(void)
{
   Eet_Data_Descriptor *d;

   Eet_Data_Descriptor_Class eddc;
   EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, app_closed_st);
   d = eet_data_descriptor_stream_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_BASIC (d, app_closed_st, "ptr",
         ptr, EET_T_ULONG_LONG);

   return d;
}

Eet_Data_Descriptor *
highlight_desc_make(void)
{
   Eet_Data_Descriptor *d;

   Eet_Data_Descriptor_Class eddc;
   EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, highlight_st);
   d = eet_data_descriptor_stream_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_BASIC (d, highlight_st, "app",
         app, EET_T_ULONG_LONG);
   EET_DATA_DESCRIPTOR_ADD_BASIC (d, highlight_st,
         "object", object, EET_T_ULONG_LONG);

   return d;
}

Eet_Data_Descriptor *
bmp_req_desc_make(void)
{
   Eet_Data_Descriptor *d;

   Eet_Data_Descriptor_Class eddc;
   EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, bmp_req_st);
   d = eet_data_descriptor_stream_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_BASIC (d, bmp_req_st, "gui",
         gui, EET_T_ULONG_LONG);
   EET_DATA_DESCRIPTOR_ADD_BASIC (d, bmp_req_st, "app",
         app, EET_T_ULONG_LONG);
   EET_DATA_DESCRIPTOR_ADD_BASIC (d, bmp_req_st,
         "object", object, EET_T_ULONG_LONG);
   EET_DATA_DESCRIPTOR_ADD_BASIC (d, bmp_req_st, "ctr",
         ctr, EET_T_UINT);

   return d;
}

Eet_Data_Descriptor *
tree_item_desc_make(void)
{
   Eet_Data_Descriptor *d;

   Eet_Data_Descriptor_Class eddc;
   EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, Tree_Item);
   d = eet_data_descriptor_stream_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_LIST(d, Tree_Item, "children",
         children, d);
   EET_DATA_DESCRIPTOR_ADD_BASIC(d, Tree_Item, "name",
         name, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(d, Tree_Item, "ptr",
         ptr, EET_T_UINT);
   EET_DATA_DESCRIPTOR_ADD_SUB(d, Tree_Item, "info",
         info, desc->obj_info);
   EET_DATA_DESCRIPTOR_ADD_BASIC(d, Tree_Item, "is_obj",
         is_obj, EET_T_UCHAR);
   EET_DATA_DESCRIPTOR_ADD_BASIC(d, Tree_Item, "is_clipper",
         is_clipper, EET_T_UCHAR);
   EET_DATA_DESCRIPTOR_ADD_BASIC(d, Tree_Item, "is_visible",
         is_visible, EET_T_UCHAR);

   return d;
}

data_desc *
data_descriptors_init(void)
{
   if (desc)  /* Was allocated */
     return desc;

   desc = calloc(1, sizeof(data_desc));

   Eet_Data_Descriptor_Class eddc;

   desc->bmp_req    = bmp_req_desc_make();
   desc->bmp_info   = bmp_info_desc_make();
   desc->shot_list   = shot_list_desc_make();
   desc->obj_info   = Obj_Information_desc_make();
   desc->tree       = tree_item_desc_make();
   desc->connect    = connect_desc_make();
   desc->app_add    = app_add_desc_make();
   desc->data_req   = data_req_desc_make();
   desc->tree_data  = tree_data_desc_make();
   desc->app_closed = app_closed_desc_make();
   desc->highlight  = highlight_desc_make();

   /* for variant */
   EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, Variant_st);
   desc->_variant_descriptor = eet_data_descriptor_file_new(&eddc);

   eddc.version = EET_DATA_DESCRIPTOR_CLASS_VERSION;
   eddc.func.type_get = _variant_type_get;
   eddc.func.type_set = _variant_type_set;
   desc->_variant_unified_descriptor = eet_data_descriptor_stream_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_MAPPING(desc->_variant_unified_descriptor,
        GUI_CONNECT_STR, desc->connect);

   EET_DATA_DESCRIPTOR_ADD_MAPPING(desc->_variant_unified_descriptor,
        APP_CONNECT_STR, desc->connect);

   EET_DATA_DESCRIPTOR_ADD_MAPPING(desc->_variant_unified_descriptor,
        APP_ADD_STR , desc->app_add);

   EET_DATA_DESCRIPTOR_ADD_MAPPING(desc->_variant_unified_descriptor,
        DATA_REQ_STR, desc->data_req);

   EET_DATA_DESCRIPTOR_ADD_MAPPING(desc->_variant_unified_descriptor,
        TREE_DATA_STR, desc->tree_data);

   EET_DATA_DESCRIPTOR_ADD_MAPPING(desc->_variant_unified_descriptor,
        APP_CLOSED_STR, desc->app_closed);

   EET_DATA_DESCRIPTOR_ADD_MAPPING(desc->_variant_unified_descriptor,
        HIGHLIGHT_STR, desc->highlight);

   EET_DATA_DESCRIPTOR_ADD_MAPPING(desc->_variant_unified_descriptor,
        BMP_REQ_STR, desc->bmp_req);

   EET_DATA_DESCRIPTOR_ADD_MAPPING(desc->_variant_unified_descriptor,
        BMP_DATA_STR, desc->bmp_info);

   EET_DATA_DESCRIPTOR_ADD_VARIANT(desc->_variant_descriptor,
         Variant_st, "data", data, t, desc->_variant_unified_descriptor);

   return desc;
}

void
data_descriptors_shutdown(void)
{
   Obj_Information_desc_shutdown();

   if (desc)
     {
        eet_data_descriptor_free(desc->connect);
        eet_data_descriptor_free(desc->app_add);
        eet_data_descriptor_free(desc->data_req);
        eet_data_descriptor_free(desc->tree_data);
        eet_data_descriptor_free(desc->app_closed);
        eet_data_descriptor_free(desc->highlight);
        eet_data_descriptor_free(desc->tree);
        eet_data_descriptor_free(desc->obj_info);
        eet_data_descriptor_free(desc->_variant_descriptor );
        eet_data_descriptor_free(desc->_variant_unified_descriptor);
        eet_data_descriptor_free(desc->bmp_req);
        eet_data_descriptor_free(desc->bmp_info);
        eet_data_descriptor_free(desc->shot_list);

        free(desc);
        desc = NULL;
     }
}

static void *
_host_to_net_blob_get(void *blob, int *blob_size)
{
   if (!blob)
     return blob;

   /* Add extra long if needed */
   if ((*blob_size) % sizeof(uint32_t))
     (*blob_size)++;

   void *n_blob = malloc(*blob_size);
   uint32_t *src = blob;
   uint32_t *dst = n_blob;
   int cnt = (*blob_size) / sizeof(uint32_t);
   while (cnt)
     {
        *dst = htonl(*src);
        src++;
        dst++;
        cnt--;
     }

   return n_blob;
}

static void *
_net_to_host_blob_get(void *blob, int blob_size)
{
   if (!blob)
     return blob;

   void *h_blob = malloc(blob_size);

   uint32_t *src = blob;
   uint32_t *dst = h_blob;
   int cnt = blob_size / sizeof(uint32_t);
   while (cnt)
     {
        *dst = ntohl(*src);
        src++;
        dst++;
        cnt--;
     }

   return h_blob;
}

void *
packet_compose(message_type t, void *data,
      int data_size, int *size,
      void *blob, int blob_size)
{  /* Returns packet BLOB and size in size param, NULL on failure */
   /* Packet is composed of packet type BYTE + packet data.       */
   void *p = NULL;
   void *pb = NULL;
   unsigned char p_type = VARIANT_PACKET;
   data_desc *d = data_descriptors_init();

   switch (t)
     {
      case BMP_DATA:
           {  /* Builed Bitmap data as follows:
                 First we have encoding size of bmp_info_st
                 (Done Network Byte Order)
                 The next to come will be the encoded bmp_info_st itself
                 folloed by the Bitmap raw data.                          */

              /* First, we like to encode bmp_info_st from data  */
              int e_size;
              Variant_st *v = variant_alloc(t, data_size, data);
              p = eet_data_descriptor_encode(
                    d->_variant_descriptor , v, &e_size);
              variant_free(v);

              /* Save encoded size in network format */
              uint32_t enc_size = htonl((uint32_t) e_size);
              void *net_blob = _host_to_net_blob_get(blob, &blob_size);


              /* Update size to the buffer size we about to return */
              *size = (e_size + blob_size + sizeof(enc_size));
              char *b = malloc(*size);

              /* Copy encoded-size first, followd by encoded data */
              memcpy(b, &enc_size, sizeof(enc_size));
              memcpy(b + sizeof(enc_size), p, e_size);
              free(p);

              if (net_blob)    /* Copy BMP info */
                {
                   memcpy(b + sizeof(enc_size) + e_size, net_blob, blob_size);
                   free(net_blob);
                }

              p_type = BMP_RAW_DATA;
              p = b;
           }
         break;

      default:
           {  /* All others are variant packets with EET encoding  */
              /* Variant is composed of message type + ptr to data */
              Variant_st *v = variant_alloc(t, data_size, data);
              p = eet_data_descriptor_encode(
                    d->_variant_descriptor ,v, size);
              variant_free(v);
           }
     }

   pb = malloc((*size) + 1);
   *((unsigned char *) pb) = p_type;
   memcpy(((char *) pb) + 1, p, *size);
   *size = (*size) + 1;  /* Add space for packet type */
   free(p);

   return pb;  /* User has to free(pb) */
}

Variant_st *
packet_info_get(void *data, int size)
{  /* user has to use variant_free() to free return struct */
   char *ch = data;
   data_desc *d = data_descriptors_init();

   switch (*ch)
     {
      case BMP_RAW_DATA:
           {
              void *bmp = NULL;
              char *b = (char *) (ch + 1);
              uint32_t enc_size = ntohl(*(uint32_t *) b);
              /* blob_size is total size minus 1st byte and enc_size size */
              int blob_size = size - (enc_size + 1 + sizeof(enc_size));

              if (blob_size)
                {  /* Allocate BMP only if was included in packet */
                   bmp = _net_to_host_blob_get(
                         b + sizeof(enc_size) + enc_size, blob_size);
                }

              /* User have to free both: variant_free(rt), free(t->bmp) */
              Variant_st *v = eet_data_descriptor_decode(d->_variant_descriptor,
                    b + sizeof(enc_size), enc_size);

              bmp_info_st *st = v->data;
              st->bmp = bmp;
              return v;
           }
         break;

      default:
           {
              return eet_data_descriptor_decode(d->_variant_descriptor,
                    ch + 1, size - 1);
           }
     }
}

Eina_Bool
eet_info_save(const char *filename,
      app_info_st *a, tree_data_st *ftd, Eina_List *ck_list)
{
   data_desc *d = data_descriptors_init();
   Eet_File *fp = eet_open(filename, EET_FILE_MODE_WRITE);
   if (fp)
     {
        eet_data_write(fp, d->app_add, APP_ADD_ENTRY, a, EINA_TRUE);
        eet_data_write(fp, d->tree_data, TREE_DATA_ENTRY, ftd, EINA_TRUE);

        /* Build list of (bmp_info_st *) according to user selection    */
        shot_list_st t;
        Eina_List *l;
        Evas_Object *ck;
        t.view = NULL;
        EINA_LIST_FOREACH(ck_list, l , ck)
           if (elm_check_state_get(ck) && evas_object_data_get(ck, BMP_FIELD))
             t.view = eina_list_append(t.view,
                   evas_object_data_get(ck, BMP_FIELD));

        if (t.view)
          {  /* Write list and bitmaps */
             char buf[1024];
             bmp_info_st *st = NULL;
             eet_data_write(fp, d->shot_list, BMP_LIST_ENTRY, &t, EINA_TRUE);
             EINA_LIST_FOREACH(t.view, l , st)
               {
                  sprintf(buf, "%s/%llx", BMP_DATA_ENTRY, st->object);
                  eet_data_image_write(fp, buf,
                        st->bmp, st->w, st->h, 1, 0, 100, 0);
               }

             eina_list_free(t.view);
          }

        eet_close(fp);

        return EINA_TRUE;
     }

   return EINA_FALSE;
}

Eina_Bool eet_info_read(const char *filename,
      app_info_st **a, tree_data_st **ftd)
{
   data_desc *d = data_descriptors_init();
   Eet_File *fp = eet_open(filename, EET_FILE_MODE_READ);
   if (fp)
     {
        *a = eet_data_read(fp, d->app_add, APP_ADD_ENTRY);
        *ftd = eet_data_read(fp, d->tree_data, TREE_DATA_ENTRY);
        shot_list_st *t = eet_data_read(fp, d->shot_list, BMP_LIST_ENTRY);
        if (t->view)
          {
             Eina_List *l;
             bmp_info_st *st = NULL;
             EINA_LIST_FOREACH(t->view, l , st)
               {
                  char buf[1024];
                  int alpha;
                  int compress;
                  int quality;
                  int lossy;

                  sprintf(buf, "%s/%llx", BMP_DATA_ENTRY, st->object);
                  st->bmp = eet_data_image_read(fp, buf,
                        (unsigned int *) &st->w, (unsigned int *) &st->h,
                        &alpha, &compress, &quality, &lossy);

                  /* Add the bitmaps to the actuall app data struct */
                  Variant_st *v = variant_alloc(BMP_DATA, sizeof(*st), st);
                  (*a)->view = eina_list_append((*a)->view, v);
               }

             eina_list_free(t->view);
          }

        free(t);
        eet_close(fp);

        return EINA_TRUE;
     }

   return EINA_FALSE;
}


/* HIGHLIGHT code. */
static Eina_Bool
libclouseau_highlight_fade(void *_rect)
{
   Evas_Object *rect = _rect;
   int r, g, b, a;
   double na;

   evas_object_color_get(rect, &r, &g, &b, &a);
   if (a < 20)
     {
        evas_object_del(rect);
        return EINA_FALSE;
     }

   na = a - 20;
   r = na / a * r;
   g = na / a * g;
   b = na / a * b;
   evas_object_color_set(rect, r, g, b, na);

   return EINA_TRUE;
}

static Evas_Object *
_verify_e_children(Evas_Object *obj, Evas_Object *ptr)
{  /* Verify that obj still exists (can be found on evas canvas) */
   if (ptr == obj)
     return ptr;

   Evas_Object *child, *p = NULL;
   Eina_List *tmp, *children = evas_object_smart_members_get(obj);
   EINA_LIST_FOREACH(children, tmp, child)
     {
        p = _verify_e_children(child, ptr);
        if (p) break;
     }

   eina_list_free(children);
   return p;
}

static Evas_Object *
_verify_e_obj(Evas_Object *obj)
{  /* Verify that obj still exists (can be found on evas canvas) */
   Evas_Object *o;
   Eina_List *eeitr ,*objitr, *ees = ecore_evas_ecore_evas_list_get();
   Ecore_Evas *ee;
   Evas_Object *rt = NULL;

   EINA_LIST_FOREACH(ees, eeitr, ee)
     {
        Evas *e = ecore_evas_get(ee);
        Eina_List *objs = evas_objects_in_rectangle_get(e, SHRT_MIN, SHRT_MIN,
              USHRT_MAX, USHRT_MAX, EINA_TRUE, EINA_TRUE);

        EINA_LIST_FOREACH(objs, objitr, o)
          {
             rt = _verify_e_children(o, obj);
             if (rt) break;
          }

        eina_list_free(objs);
        if (rt) break;
     }

   eina_list_free(ees);
   return rt;
}

static void
libclouseau_highlight_del(void *data,
      EINA_UNUSED Evas *e,
      EINA_UNUSED  Evas_Object *obj,
      EINA_UNUSED  void *event_info)
{  /* Delete timer for this rect */
   ecore_timer_del(data);
}

void
libclouseau_highlight(Evas_Object *obj, st_evas_props *props, bmp_info_st *view)
{
   Evas_Object *r;
   int x, y, wd, ht;
   Evas *e = NULL;

   if (props)
     {  /* When working offline grab info from struct */
        Evas_Coord x_bmp, y_bmp;
        evas_object_geometry_get(view->o, &x_bmp, &y_bmp, NULL, NULL);
        x =  (view->zoom_val * props->x) + x_bmp;
        y =  (view->zoom_val * props->y) + y_bmp;
        wd = (view->zoom_val * props->w);
        ht = (view->zoom_val * props->h);

        e = evas_object_evas_get(view->win);
     }
   else
     {  /* Check validity of object when working online */
        if (_verify_e_obj(obj))
          evas_object_geometry_get(obj, &x, &y, &wd, &ht);
        else
          {
             printf("<%s> Evas Object not found <%p> (probably deleted)\n",
                   __func__, obj);
             return;
          }

        /* Take evas from object if working online */
        e = evas_object_evas_get(obj);
        if (!e) return;
     }

   /* Continue and do the Highlight */
   r = evas_object_rectangle_add(e);
   evas_object_move(r, x - PADDING, y - PADDING);
   evas_object_resize(r, wd + (2 * PADDING), ht + (2 * PADDING));
   evas_object_color_set(r, HIGHLIGHT_R, HIGHLIGHT_G, HIGHLIGHT_B,
         HIGHLIGHT_A);
   evas_object_show(r);

   /* Add Timer for fade and a callback to delete timer on obj del */
   evas_object_event_callback_add(r, EVAS_CALLBACK_DEL,
         libclouseau_highlight_del,
         ecore_timer_add(0.1, libclouseau_highlight_fade, r));
/* Print backtrace info, saved for future ref
   tmp = evas_object_data_get(obj, ".clouseau.bt");
   fprintf(stderr, "Creation backtrace :\n%s*******\n", tmp); */
}

void
libclouseau_make_lines(bmp_info_st *st, Evas_Coord xx, Evas_Coord yy)
{  /* and no, we are NOT talking about WHITE lines */
   Evas_Coord x_rgn, y_rgn, w_rgn, h_rgn;
   lines_free(st);

   elm_scroller_region_get(st->scr, &x_rgn, &y_rgn, &w_rgn, &h_rgn);

   st->lx = evas_object_line_add(evas_object_evas_get(st->o));
   st->ly = evas_object_line_add(evas_object_evas_get(st->o));
   evas_object_repeat_events_set(st->lx, EINA_TRUE);
   evas_object_repeat_events_set(st->ly, EINA_TRUE);

   evas_object_line_xy_set(st->lx, 0, yy, w_rgn, yy);
   evas_object_line_xy_set(st->ly, xx, 0, xx, h_rgn);

   evas_object_color_set(st->lx, HIGHLIGHT_R, HIGHLIGHT_G, HIGHLIGHT_B,
         HIGHLIGHT_A);
   evas_object_color_set(st->ly, HIGHLIGHT_R, HIGHLIGHT_G, HIGHLIGHT_B,
         HIGHLIGHT_A);
   evas_object_show(st->lx);
   evas_object_show(st->ly);
}

void
libclouseau_lines_cb(void *data,
      Evas *e EINA_UNUSED, Evas_Object *obj EINA_UNUSED,
      void *event_info)
{
   if (((Evas_Event_Mouse_Down *) event_info)->button == 1)
     return; /* Draw line only if not left mouse button */

   libclouseau_make_lines(data, 
         (((Evas_Event_Mouse_Move *) event_info)->cur.canvas.x),
         (((Evas_Event_Mouse_Move *) event_info)->cur.canvas.y));
}
