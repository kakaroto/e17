#include "Evoak.h"
#include "evoak_private.h"

/* TODO: Split into separate edje object types
 * TODO: Split out callbacks, signals, etc.
 * TODO: Pull in the protocol code for each type
 * TODO: Server code for each type?
*/

static void _evoak_object_edje_file_load(Evoak_Meta_Edje *m);
static void _evoak_object_edje_file_unload(Evoak_Meta_Edje *m);
static void _evoak_object_edje_file_free(Edje_File *edf);
static void _evoak_object_edje_collection_free(Edje_Part_Collection *ec);
static void _evoak_object_edje_collection_free_part_description_free(Edje_Part_D
escription *desc);

extern Eet_Data_Descriptor *_edje_edd_edje_file;
extern Eet_Data_Descriptor *_edje_edd_edje_data;
extern Eet_Data_Descriptor *_edje_edd_edje_image_directory;
extern Eet_Data_Descriptor *_edje_edd_edje_image_directory_entry;
extern Eet_Data_Descriptor *_edje_edd_edje_program;
extern Eet_Data_Descriptor *_edje_edd_edje_program_target;
extern Eet_Data_Descriptor *_edje_edd_edje_program_after;
extern Eet_Data_Descriptor *_edje_edd_edje_part_collection_directory;
extern Eet_Data_Descriptor *_edje_edd_edje_part_collection_directory_entry;
extern Eet_Data_Descriptor *_edje_edd_edje_part_collection;
extern Eet_Data_Descriptor *_edje_edd_edje_part;
extern Eet_Data_Descriptor *_edje_edd_edje_part_description;
extern Eet_Data_Descriptor *_edje_edd_edje_part_image_id;

static Evas_Hash *_evoak_edje_file_hash = NULL;

static void
_evoak_object_edje_file_load(Evoak_Meta_Edje *m)
{
   Eet_File *ef = NULL;
   Evas_List *l;
   int id = -1;

   m->load_error = 0;
   ef = eet_open(m->file, EET_FILE_MODE_READ);
   if (!ef)
     {
        m->load_error = 1;
        return;
     }

   m->edje.file = eet_data_read(ef, _edje_edd_edje_file, "edje_file");
   if (!m->edje.file)
     {
        m->load_error = 1;
        goto out;
     }

   m->edje.file->references = 1;
   m->edje.file->path = strdup(m->file);
   if (!m->edje.file->collection_dir)
     {
        _evoak_object_edje_file_free(m->edje.file);
        m->edje.file = NULL;
        m->load_error = 1;
        goto out;
     }
   _evoak_edje_file_hash =
     evas_hash_add(_evoak_edje_file_hash,
                   m->edje.file->path,
                   m->edje.file);
   m->edje.collection =
     evas_hash_find(m->edje.file->collection_hash, m->group);
   if (m->edje.collection)
     m->edje.collection->references++;
   else
     {
        for (l = m->edje.file->collection_dir->entries; l; l = l->next)
          {
             Edje_Part_Collection_Directory_Entry *ce;

             ce = l->data;
             if ((ce->entry) && (!strcmp(ce->entry, m->group)))
               {
                  id = ce->id;
                  break;
               }
          }
        if (id >= 0)
          {
             char buf[256];
             int  size;
             void *data;

             snprintf(buf, sizeof(buf), "collections/%i", id);
             if (!ef) ef = eet_open(m->file, EET_FILE_MODE_READ);
             if (!ef)
               {
                  m->edje.file->references--;
                  if (m->edje.file->references <= 0)
                    {
                       _evoak_edje_file_hash =
                         evas_hash_del(_evoak_edje_file_hash, m->file, m->edje.f
ile);
                       _evoak_object_edje_file_free(m->edje.file);
                    }
                  m->edje.file = NULL;
                  m->load_error = 1;
                  goto out;
               }
             m->edje.collection =
               eet_data_read(ef,
                             _edje_edd_edje_part_collection,
                             buf);
             if (!m->edje.collection)
               {
                  m->edje.file->references--;
                  if (m->edje.file->references <= 0)
                    {
                       _evoak_edje_file_hash =
                         evas_hash_del(_evoak_edje_file_hash, m->file, m->edje.f
ile);
                       _evoak_object_edje_file_free(m->edje.file);
                    }
                  m->edje.file = NULL;
                  m->load_error = 1;
                  goto out;
               }
             m->edje.collection->references = 1;
             m->edje.file->collection_hash =
               evas_hash_add(m->edje.file->collection_hash,
                             m->group,
                             m->edje.collection);
          }
        else
          {
             m->edje.file->references--;
             if (m->edje.file->references <= 0)
               {
                  _evoak_edje_file_hash =
                    evas_hash_del(_evoak_edje_file_hash, m->file, m->edje.file);
                  _evoak_object_edje_file_free(m->edje.file);
               }
             m->edje.file = NULL;
             m->load_error = 1;
          }
     }
   out:
   if (ef) eet_close(ef);
   /* build part list shadow */
   if (m->edje.collection)
     {
        for (l = m->edje.collection->parts; l; l = l->next)
          {
             Edje_Part *ep;
             Evoak_Meta_Edje_Part *evp;

             ep = l->data;
             evp = calloc(1, sizeof(Evoak_Meta_Edje_Part));
             evp->part = ep;
             m->parts = evas_list_append(m->parts, evp);
          }
     }
}

static void
_evoak_object_edje_file_unload(Evoak_Meta_Edje *m)
{
   while (m->parts)
     {
        Evoak_Meta_Edje_Part *evp;

        evp = m->parts->data;
        if (evp->swallow)
          evoak_object_edje_part_unswallow(m->obj, evp->swallow);
        if (evp->text) free(evp->text);
        if (evp->state_name) free(evp->state_name);
        free(evp);
        m->parts = evas_list_remove_list(m->parts, m->parts);
     }
   if (m->edje.collection)
     {
        m->edje.collection->references--;
        if (m->edje.collection->references <= 0)
          {
             m->edje.file->collection_hash =
               evas_hash_del(m->edje.file->collection_hash,
                             m->group,
                             m->edje.collection);
             m->edje.collection->references--;
             _evoak_object_edje_collection_free(m->edje.collection);
          }
        m->edje.collection = NULL;
     }
   if (m->edje.file)
     {
        m->edje.file->references--;
        if (m->edje.file->references <= 0)
          {
             _evoak_edje_file_hash =
               evas_hash_del(_evoak_edje_file_hash, m->file, m->edje.file);
             _evoak_object_edje_file_free(m->edje.file);
          }
        m->edje.file = NULL;
     }
}

static void
_evoak_object_edje_file_free(Edje_File *edf)
{
   if (edf->path) free(edf->path);
   if (edf->image_dir)
     {
        while (edf->image_dir->entries)
          {
             Edje_Image_Directory_Entry *ie;

             ie = edf->image_dir->entries->data;
             edf->image_dir->entries =
               evas_list_remove(edf->image_dir->entries, ie);
             if (ie->entry) free(ie->entry);
             free(ie);
          }
        free(edf->image_dir);
     }
   if (edf->collection_dir)
     {
        while (edf->collection_dir->entries)
          {
             Edje_Part_Collection_Directory_Entry *ce;

             ce = edf->collection_dir->entries->data;
             edf->collection_dir->entries =
               evas_list_remove(edf->collection_dir->entries, ce);
             if (ce->entry) free(ce->entry);
             free(ce);
          }
        free(edf->collection_dir);
     }
   if (edf->collection_hash) evas_hash_free(edf->collection_hash);
   free(edf);
}

static void
_evoak_object_edje_collection_free(Edje_Part_Collection *ec)
{
   while (ec->programs)
     {
        Edje_Program *pr;

        pr = ec->programs->data;
        ec->programs = evas_list_remove(ec->programs, pr);
        if (pr->name) free(pr->name);
        if (pr->signal) free(pr->signal);
        if (pr->source) free(pr->source);
        if (pr->state) free(pr->state);
        if (pr->state2) free(pr->state2);
        while (pr->targets)
          {
             Edje_Program_Target *prt;

             prt = pr->targets->data;
             pr->targets = evas_list_remove(pr->targets, prt);
             free(prt);
          }
        while (pr->after)
          {
             Edje_Program_After *pa;

             pa = pr->after->data;
             pr->after = evas_list_remove(pr->after, pa);
             free(pa);
          }
        free(pr);
     }
   while (ec->parts)
     {
        Edje_Part *ep;

        ep = ec->parts->data;
        ec->parts = evas_list_remove(ec->parts, ep);
        if (ep->name) free(ep->name);
        if (ep->default_desc)
          {
             _evoak_object_edje_collection_free_part_description_free(ep->defaul
t_desc);
             ep->default_desc = NULL;
          }
        while (ep->other_desc)
          {
             Edje_Part_Description *desc;

             desc = ep->other_desc->data;
             ep->other_desc = evas_list_remove(ep->other_desc, desc);
             _evoak_object_edje_collection_free_part_description_free(desc);
          }
        free(ep);
     }
#ifdef EDJE_PROGRAM_CACHE
   if (ec->prog_cache.no_matches) evas_hash_free(ec->prog_cache.no_matches);
   if (ec->prog_cache.matches)
     {
        evas_hash_foreach(ec->prog_cache.matches,
                          _edje_collection_free_prog_cache_matches_free_cb,
                          NULL);
        evas_hash_free(ec->prog_cache.matches);
     }
#endif
   free(ec);
}

static void
_evoak_object_edje_collection_free_part_description_free(Edje_Part_Description *
desc)
{
   if (desc->state.name) free(desc->state.name);
   while (desc->image.tween_list)
     {
        Edje_Part_Image_Id *pi;

        pi = desc->image.tween_list->data;
        desc->image.tween_list = evas_list_remove(desc->image.tween_list, pi);
        free(pi);
     }
   if (desc->color_class)     free(desc->color_class);
   if (desc->text.text)       free(desc->text.text);
   if (desc->text.text_class) free(desc->text.text_class);
   if (desc->text.font)       free(desc->text.font);
   free(desc);
}

static void *
_evoak_object_edje_meta_new(Evoak_Object *o)
{
   Evoak_Meta_Edje *m;

   m = calloc(1, sizeof(Evoak_Meta_Edje));
   if (!m) return NULL;
   m->obj = o;
   return m;
}

static void
_evoak_object_edje_meta_free(void *meta)
{
   Evoak_Meta_Edje *m;

   m = meta;
   _evoak_object_edje_file_unload(m);
   if (m->file) free(m->file);
   if (m->group) free(m->group);
   while (m->callbacks)
     {
        Evoak_Meta_Edje_Callback *cb;

        cb = m->callbacks->data;
        free(cb->emission);
        free(cb->source);
        free(cb);
        m->callbacks = evas_list_remove(m->callbacks, m->callbacks->data);
     }
   free(m);
}

/*
 * Create a new edje object
 * @param ev The evoak server connection on whhich to create the object
 * @return A new object handle
 * 
 * This function will create a new edje object on the server and return a
 * pointer to a local object structure that will be used to address the
 * remotely created object. On failure a NULL pointer is returned.
 */
Evoak_Object *
evoak_object_edje_add(Evoak *ev)
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
   o->magic2 = EVOAK_OBJECT_EDJE_MAGIC;
   o->type = "edje";
   o->r = 255; o->g = 255; o->b = 255; o->a = 255;
   p.type = EVOAK_OBJECT_TYPE_EDJE;
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
   o->meta = _evoak_object_edje_meta_new(o);
   o->meta_free = _evoak_object_edje_meta_free;
   return o;
}

int
evoak_object_edje_file_set(Evoak_Object *o, const char *file, const char *part)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return 0;
   if (o->magic2 != EVOAK_OBJECT_EDJE_MAGIC) return 0;
   if (o->delete_me) return 0;
   if (!file) file = "";
   if (!part) part = "";
     {
        Evoak_Meta_Edje *m;

        m = o->meta;
        if ((file) && (m->file) && (!strcmp(m->file, file)))
          {
             if ((m->group) && (part) && (!strcmp(m->group, part))) return 1;
          }
        if (m->file)
          {
             free(m->file);
             m->file = NULL;
          }
        if (m->group)
          {
             free(m->group);
             m->group = NULL;
          }
        if (file) m->file = strdup(file);
        if (part) m->group = strdup(part);
        _evoak_object_edje_file_unload(m);
        _evoak_object_edje_file_load(m);
          {
             unsigned char *d;
             int s;
             Evoak_PR_Object_Edje_File_Set p;

             p.file = (char *)file;
             p.group = (char *)part;
             d = _evoak_proto[EVOAK_PR_OBJECT_EDJE_FILE_SET].enc(&p, &s);
             if (d)
               {
                  _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_EDJE_FILE_SET, o->id
, 0, d, s);
                  free(d);
               }
          }
        if (m->load_error) return 0;
        return 1;
     }
   return 0;
}

void
evoak_object_edje_file_get(Evoak_Object *o, const char **file, const char **part
)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->magic2 != EVOAK_OBJECT_EDJE_MAGIC) return;
   if (o->delete_me) return;
     {
        Evoak_Meta_Edje *m;

        m = o->meta;
        if (file) *file = m->file;
        if (part) *part = m->group;
     }
}

const char *
evoak_object_edje_data_get(Evoak_Object *o, const char *key)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return NULL;
   if (o->magic2 != EVOAK_OBJECT_EDJE_MAGIC) return NULL;
   if (o->delete_me) return NULL;
     {
        Evoak_Meta_Edje *m;
        Evas_List *l;

        m = o->meta;
        for (l = m->edje.collection->data; l; l = l->next)
          {
             Edje_Data *di;

             di = l->data;
             if (!strcmp(di->key, key)) return (const char *)di->value;
          }
     }
   return NULL;
}

void
evoak_object_edje_signal_callback_add(Evoak_Object *o, const char *emission, con
st char *source, void (*func) (void *data, Evoak_Object *o, const char *emission
, const char *source), void *data)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->magic2 != EVOAK_OBJECT_EDJE_MAGIC) return;
   if (o->delete_me) return;
   if ((!emission) || (!source)) return;
     {
        Evoak_Meta_Edje *m;
        Evoak_Meta_Edje_Callback *cb;

        m = o->meta;
        m->callback_id++;
        cb = calloc(1, sizeof(Evoak_Meta_Edje_Callback));
        if (!cb) return;
        cb->id = m->callback_id;
        cb->emission = strdup(emission);
        cb->source = strdup(source);
        cb->func = func;
        cb->data = data;
        m->callbacks = evas_list_append(m->callbacks, cb);
          {
             unsigned char *d;
             int s;
             Evoak_PR_Object_Edje_Signal_Listen p;

             p.callback_id = cb->id;
             p.emission = (char *)emission;
             p.source = (char *)source;
             d = _evoak_proto[EVOAK_PR_OBJECT_EDJE_SIGNAL_LISTEN].enc(&p, &s);
             if (d)
               {
                  _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_EDJE_SIGNAL_LISTEN,
o->id, 0, d, s);
                  free(d);
               }
          }
     }
}

void *
evoak_object_edje_signal_callback_del(Evoak_Object *o, const char *emission, con
st char *source, void (*func) (void *data, Evoak_Object *o, const char *emission
, const char *source))
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return NULL;
   if (o->magic2 != EVOAK_OBJECT_EDJE_MAGIC) return NULL;
   if (o->delete_me) return NULL;
     {
        Evoak_Meta_Edje *m;
        Evas_List *l;

        m = o->meta;
        for (l = m->callbacks; l; l = l->next)
          {
             Evoak_Meta_Edje_Callback *cb;

             cb = l->data;
             if (((((emission) && (!strcmp(cb->emission, emission))) || (!emissi
on)) &&
                  (((source) && (!strcmp(cb->source, source))) || (!source))) &&
                 ((cb->func == func) || (!func)))
               {
                  void *data;

                    {
                       unsigned char *d;
                       int s;
                       Evoak_PR_Object_Edje_Signal_Unlisten p;

                       p.callback_id = cb->id;
                       p.emission = (char *)emission;
                       p.source = (char *)source;
                       d = _evoak_proto[EVOAK_PR_OBJECT_EDJE_SIGNAL_UNLISTEN].en
c(&p, &s);
                       if (d)
                         {
                            _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_EDJE_SIGNA
L_UNLISTEN, o->id, 0, d, s);
                            free(d);
                         }
                    }
                  data = cb->data;
                  free(cb->emission);
                  free(cb->source);
                  free(cb);
                  m->callbacks = evas_list_remove_list(m->callbacks, l);
                  return data;
               }
          }
     }
   return NULL;
}

void
evoak_object_edje_signal_emit(Evoak_Object *o, const char *emission, const char
*source)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->magic2 != EVOAK_OBJECT_EDJE_MAGIC) return;
   if (o->delete_me) return;
   if ((!emission) || (!source)) return;
     {
        unsigned char *d;
        int s;
        Evoak_PR_Object_Edje_Signal_Emit p;

        p.emission = (char *)emission;
        p.source = (char *)source;
        d = _evoak_proto[EVOAK_PR_OBJECT_EDJE_SIGNAL_EMIT].enc(&p, &s);
        if (d)
          {
             _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_EDJE_SIGNAL_EMIT, o->id,
0, d, s);
             free(d);
          }
     }
}

int
evoak_object_edje_part_exists(Evoak_Object *o, const char *part)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return 0;
   if (o->magic2 != EVOAK_OBJECT_EDJE_MAGIC) return 0;
   if (o->delete_me) return 0;
   if (!part) return 0;
     {
        Evoak_Meta_Edje *m;
        Evas_List *l;

        m = o->meta;
        for (l = m->edje.collection->parts; l; l = l->next)
          {
             Edje_Part *ep;

             ep = l->data;
             if (!strcmp(ep->name, part)) return 1;
          }
     }
   return 0;
}

void
evoak_object_edje_part_swallow(Evoak_Object *o, const char *part, Evoak_Object *
o_swallow)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->magic2 != EVOAK_OBJECT_EDJE_MAGIC) return;
   if (o->delete_me) return;
   if ((!o_swallow) || (o_swallow->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o_swallow->delete_me) return;
     {
        Evas_List *l;
        Evoak_Meta_Edje *m;

        m = o->meta;
        for (l = m->parts; l; l = l->next)
          {
             Evoak_Meta_Edje_Part *evp;

             evp = l->data;
             if (!strcmp(evp->part->name, part))
               {
                  if (evp->swallow)
                    evoak_object_edje_part_unswallow(o, evp->swallow);
                  evp->swallow = o_swallow;
                    {
                       unsigned char *d;
                       int s;
                       Evoak_PR_Object_Edje_Swallow p;

                       p.swallow_id = o_swallow->id;
                       p.part = (char *)part;
                       d = _evoak_proto[EVOAK_PR_OBJECT_EDJE_SWALLOW].enc(&p, &s
);
                       if (d)
                         {
                            _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_EDJE_SWALL
OW, o->id, 0, d, s);
                            free(d);
                         }
                    }
                  return;
               }
          }
     }
}

void
evoak_object_edje_part_unswallow(Evoak_Object *o, Evoak_Object *o_swallow)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->magic2 != EVOAK_OBJECT_EDJE_MAGIC) return;
   if (o->delete_me) return;
   if ((!o_swallow) || (o_swallow->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o_swallow->delete_me) return;
     {
        Evas_List *l;
        Evoak_Meta_Edje *m;

        m = o->meta;
        for (l = m->parts; l; l = l->next)
          {
             Evoak_Meta_Edje_Part *evp;

             evp = l->data;
             if (evp->swallow == o_swallow)
               {
                  evp->swallow = NULL;
                    {
                       unsigned char *d;
                       int s;
                       Evoak_PR_Object_Edje_Unswallow p;

                       p.swallow_id = o_swallow->id;
                       d = _evoak_proto[EVOAK_PR_OBJECT_EDJE_UNSWALLOW].enc(&p,
&s);
                       if (d)
                         {
                            _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_EDJE_UNSWA
LLOW, o->id, 0, d, s);
                            free(d);
                         }
                    }
                  o->swallowees = evas_list_remove(o->swallowees, o_swallow);
                  o_swallow->swallow = NULL;
                  return;
               }
          }
     }
}

Evoak_Object *
evoak_object_edje_part_swallow_get(Evoak_Object *o, const char *part)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return NULL;
   if (o->magic2 != EVOAK_OBJECT_EDJE_MAGIC) return NULL;
   if (o->delete_me) return NULL;
     {
        Evas_List *l;
        Evoak_Meta_Edje *m;

        m = o->meta;
        for (l = m->parts; l; l = l->next)
          {
             Evoak_Meta_Edje_Part *evp;

             evp = l->data;
             if (!strcmp(evp->part->name, part))
               return evp->swallow;
          }
     }
   return NULL;
}

void
evoak_object_edje_part_text_set(Evoak_Object *o, const char *part, const char *t
ext)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->magic2 != EVOAK_OBJECT_EDJE_MAGIC) return;
   if (o->delete_me) return;
   if ((!part) || (!text)) return;
     {
        Evas_List *l;
        Evoak_Meta_Edje *m;

        m = o->meta;
        for (l = m->parts; l; l = l->next)
          {
             Evoak_Meta_Edje_Part *evp;

             evp = l->data;
             if (!strcmp(evp->part->name, part))
               {
                    {
                       unsigned char *d;
                       int s;
                       Evoak_PR_Object_Edje_Text_Set p;

                       p.part = (char *)part;
                       p.text = (char *)text;
                       d = _evoak_proto[EVOAK_PR_OBJECT_EDJE_TEXT_SET].enc(&p, &
s);
                       if (d)
                         {
                            _evoak_ipc_send(o->evoak, EVOAK_PR_OBJECT_EDJE_TEXT_
SET, o->id, 0, d, s);
                            free(d);
                         }
                    }
                  if (evp->text) free(evp->text);
                  evp->text = strdup(text);
                  return;
               }
          }
     }
}

const char *
evoak_object_edje_part_text_get(Evoak_Object *o, const char *part)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return NULL;
   if (o->magic2 != EVOAK_OBJECT_EDJE_MAGIC) return NULL;
   if (o->delete_me) return NULL;
     {
        Evas_List *l;
        Evoak_Meta_Edje *m;

        m = o->meta;
        for (l = m->parts; l; l = l->next)
          {
             Evoak_Meta_Edje_Part *evp;

             evp = l->data;
             if (!strcmp(evp->part->name, part))
               return evp->text;
          }
     }
   return NULL;
}
