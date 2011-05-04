#include "enlil_private.h"

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#define XML_GOTO(CUR, TAG, VALUE)                                        \
  while( CUR && (VALUE = (const char *)CUR->name) && strcmp(VALUE, TAG)) \
                   CUR = CUR->next;

#define XML_DOWN(CUR)            \
  if(CUR->xmlChildrenNode)       \
    CUR = CUR->xmlChildrenNode;  \
  if(CUR) CUR = CUR->next

#define XML_UP(CUR)     \
  if(CUR->parent)       \
    CUR = CUR->parent;  \
  if(CUR) CUR = CUR->next

#define XML_NEXT(CUR)       \
  if(CUR) CUR = CUR->next;  \
  if(CUR) CUR = CUR->next

struct Enlil_Geocaching_Log
{
   const char *id;
   const char *date;
   const char *type;
   const char *finder;
   const char *finder_id;
   const char *text;
   const char *encoded;
};

struct Enlil_Geocaching_Travelbug
{
   const char *id;
   const char *ref;
   const char *name;
};

struct Enlil_Geocaching
{
   double                   longitude, latitude;
   const char              *name;
   const char              *time;
   const char              *description;
   const char              *url;
   const char              *url_name;
   const char              *sym;
   const char              *type;

   const char              *gp_name;
   const char              *gp_id;
   const char              *gp_available;
   const char              *gp_archived;
   const char              *gp_short_desc;
   const char              *gp_long_desc;
   const char              *gp_hints;
   const char              *gp_state;
   const char              *gp_placed_by;
   const char              *gp_owner;
   const char              *gp_owner_id;
   const char              *gp_type;
   const char              *gp_container;
   const char              *gp_difficulty;
   const char              *gp_terrain;
   const char              *gp_country;

   Eina_List               *logs;
   Eina_List               *tbs;

   void                    *user_data;
   Enlil_Geocaching_Free_Cb free_cb;
};

typedef struct Enlil_Geocaching_Db
{
   Eina_Bool                loaded : 1;
   Eina_Hash               *hash;
   Enlil_Geocaching_Done_Cb done_cb;
   void                    *data;
} Enlil_Geocaching_Db;

static Enlil_Geocaching_Db _db;

static Eet_Data_Descriptor *_enlil_geocaching_edd_new(Eet_Data_Descriptor *edd_log,
                                                      Eet_Data_Descriptor *edd_tb);
static Eet_Data_Descriptor *_enlil_hashtable_edd_new(Eet_Data_Descriptor *edd_gp);
static Eet_Data_Descriptor *_enlil_geocaching_log_edd_new(void);
static Eet_Data_Descriptor *_enlil_geocaching_tb_edd_new(void);
static void                 _load(void);
static int                  _save(void);

#define FCT_NAME    enlil_geocaching
#define STRUCT_TYPE Enlil_Geocaching

STRING_SET(name)
STRING_SET(gp_name)
STRING_SET(gp_short_desc)
STRING_SET(gp_long_desc)
STRING_SET(gp_placed_by)
STRING_SET(gp_owner)
STRING_SET(gp_type)
STRING_SET(gp_container)
STRING_SET(gp_difficulty)
STRING_SET(gp_terrain)
STRING_SET(gp_country)

STRING_SET(time)
STRING_SET(description)
STRING_SET(url)
STRING_SET(url_name)
STRING_SET(sym)
STRING_SET(type)
STRING_SET(gp_id)
STRING_SET(gp_available)
STRING_SET(gp_archived)
STRING_SET(gp_hints)
STRING_SET(gp_state)
STRING_SET(gp_owner_id)

SET(longitude, double)
SET(latitude, double)

GET(name, const char *)
GET(gp_name, const char *)
GET(gp_short_desc, const char *)
GET(gp_long_desc, const char *)
GET(gp_placed_by, const char *)
GET(gp_owner, const char *)
GET(gp_type, const char *)
GET(gp_container, const char *)
GET(gp_difficulty, const char *)
GET(gp_terrain, const char *)
GET(gp_country, const char *)
GET(longitude, double)
GET(latitude, double)
GET(user_data, void *)

GET(time, const char *)
GET(description, const char *)
GET(url, const char *)
GET(url_name, const char *)
GET(sym, const char *)
GET(type, const char *)
GET(gp_id, const char *)
GET(gp_available, const char *)
GET(gp_archived, const char *)
GET(gp_hints, const char *)
GET(gp_state, const char *)
GET(gp_owner_id, const char *)
GET(logs, Eina_List *)
GET(tbs, Eina_List *)

#undef FCT_NAME
#undef STRUCT_TYPE

#define FCT_NAME    enlil_geocaching_log
#define STRUCT_TYPE Enlil_Geocaching_Log

STRING_SET(id)
STRING_SET(date)
STRING_SET(type)
STRING_SET(finder)
STRING_SET(finder_id)
STRING_SET(text)
STRING_SET(encoded)

GET(id, const char *)
GET(date, const char *)
GET(type, const char *)
GET(finder, const char *)
GET(finder_id, const char *)
GET(text, const char *)
GET(encoded, const char *)

#undef FCT_NAME
#undef STRUCT_TYPE

#define FCT_NAME    enlil_geocaching_tb
#define STRUCT_TYPE Enlil_Geocaching_Travelbug

STRING_SET(id)
STRING_SET(ref)
STRING_SET(name)

GET(id, const char *)
GET(ref, const char *)
GET(name, const char *)

#undef FCT_NAME
#undef STRUCT_TYPE

Enlil_Geocaching * enlil_geocaching_new(void)
{
   Enlil_Geocaching *gp = calloc(1, sizeof(Enlil_Geocaching));

   gp->longitude = -360;
   gp->latitude = -360;
   return gp;
}

void
enlil_geocaching_free(Enlil_Geocaching *gp)
{
   Enlil_Geocaching_Travelbug *tb;
   Enlil_Geocaching_Log *log;

   ASSERT_RETURN_VOID(gp != NULL);

   if( gp->free_cb )
     gp->free_cb(gp, gp->user_data);

   EINA_STRINGSHARE_DEL(gp->name);
   EINA_STRINGSHARE_DEL(gp->time);
   EINA_STRINGSHARE_DEL(gp->description);
   EINA_STRINGSHARE_DEL(gp->url);
   EINA_STRINGSHARE_DEL(gp->url_name);
   EINA_STRINGSHARE_DEL(gp->sym);
   EINA_STRINGSHARE_DEL(gp->type);

   EINA_STRINGSHARE_DEL(gp->gp_name);
   EINA_STRINGSHARE_DEL(gp->gp_short_desc);
   EINA_STRINGSHARE_DEL(gp->gp_long_desc);
   EINA_STRINGSHARE_DEL(gp->gp_placed_by);
   EINA_STRINGSHARE_DEL(gp->gp_owner);
   EINA_STRINGSHARE_DEL(gp->gp_type);
   EINA_STRINGSHARE_DEL(gp->gp_container);
   EINA_STRINGSHARE_DEL(gp->gp_difficulty);
   EINA_STRINGSHARE_DEL(gp->gp_terrain);
   EINA_STRINGSHARE_DEL(gp->gp_country);
   EINA_STRINGSHARE_DEL(gp->gp_id);
   EINA_STRINGSHARE_DEL(gp->gp_available);
   EINA_STRINGSHARE_DEL(gp->gp_archived);
   EINA_STRINGSHARE_DEL(gp->gp_hints);
   EINA_STRINGSHARE_DEL(gp->gp_state);
   EINA_STRINGSHARE_DEL(gp->gp_owner_id);

   EINA_LIST_FREE(gp->logs, log)
     enlil_geocaching_log_free(log);

   EINA_LIST_FREE(gp->tbs, tb)
     enlil_geocaching_tb_free(tb);

   FREE(gp);
}

void
enlil_geocaching_user_data_set(Enlil_Geocaching        *gp,
                               void                    *user_data,
                               Enlil_Geocaching_Free_Cb cb)
{
   ASSERT_RETURN_VOID(gp != NULL);
   gp->user_data = user_data;
   gp->free_cb = cb;
}

Enlil_Geocaching_Log *
enlil_geocaching_log_new(void)
{
   Enlil_Geocaching_Log *gp_log = calloc(1, sizeof(Enlil_Geocaching_Log));

   return gp_log;
}

void
enlil_geocaching_log_free(Enlil_Geocaching_Log *gp_log)
{
   ASSERT_RETURN_VOID(gp_log != NULL);

   EINA_STRINGSHARE_DEL(gp_log->id);
   EINA_STRINGSHARE_DEL(gp_log->date);
   EINA_STRINGSHARE_DEL(gp_log->type);
   EINA_STRINGSHARE_DEL(gp_log->finder);
   EINA_STRINGSHARE_DEL(gp_log->finder_id);
   EINA_STRINGSHARE_DEL(gp_log->text);
   EINA_STRINGSHARE_DEL(gp_log->encoded);

   FREE(gp_log);
}

Enlil_Geocaching_Travelbug *
enlil_geocaching_tb_new(void)
{
   Enlil_Geocaching_Travelbug *gp_tb = calloc(1, sizeof(Enlil_Geocaching_Travelbug));

   return gp_tb;
}

void
enlil_geocaching_tb_free(Enlil_Geocaching_Travelbug *gp_tb)
{
   ASSERT_RETURN_VOID(gp_tb != NULL);

   EINA_STRINGSHARE_DEL(gp_tb->id);
   EINA_STRINGSHARE_DEL(gp_tb->ref);
   EINA_STRINGSHARE_DEL(gp_tb->name);

   FREE(gp_tb);
}

static Eina_Bool running = EINA_FALSE;
static void _load_thread(void         *data,
                         Ecore_Thread *thread);
static void _import_thread(void         *data,
                           Ecore_Thread *thread);
static void _import_end_cb(void         *data,
                           Ecore_Thread *thread);

void
enlil_geocaching_get(Enlil_Geocaching_Done_Cb done_cb,
                     void                    *data)
{
   ASSERT_RETURN_VOID(done_cb != NULL);

   _db.done_cb = done_cb;
   _db.data = data;

   if(_db.loaded)
   {
      _db.done_cb(_db.data, _db.hash);
      return;
   }

   ASSERT_RETURN_VOID(running != EINA_TRUE);
   running = EINA_TRUE;

   ecore_thread_run(_load_thread, _import_end_cb, NULL, NULL);
}

void
enlil_geocaching_import(const char              *file,
                        Enlil_Geocaching_Done_Cb done_cb,
                        void                    *data)
{
   ASSERT_RETURN_VOID(running != EINA_TRUE);
   ASSERT_RETURN_VOID(done_cb != NULL);
   ASSERT_RETURN_VOID(file != NULL);

   _db.done_cb = done_cb;
   _db.data = data;

   running = EINA_TRUE;

   ecore_thread_run(_import_thread, _import_end_cb, NULL, eina_stringshare_add(file));
}

static void
_load_thread(__UNUSED__ void         *data,
             __UNUSED__ Ecore_Thread *thread)
{
   if(_db.hash)
   {
      eina_hash_free(_db.hash);
      _db.hash = NULL;
   }

   _load();
}

static void
_load(void)
{
   Eet_Data_Descriptor *edd, *edd_gp, *edd_log, *edd_tb;
   Eet_File *f;
   char path[PATH_MAX];
   Enlil_Geocaching_Db *db;

   ASSERT_RETURN_VOID(_db.hash == NULL);

   snprintf(path, PATH_MAX, "%s/" EET_FOLDER_ROOT_DB, getenv("HOME"));

   LOG_INFO("Load %s\n", path);

   if(!ecore_file_exists(path))
     ecore_file_mkpath(path);
   strcat(path, EET_FILE_GROUNDSPEAK_DB);

   f = enlil_file_manager_open(path);
   ASSERT_RETURN_VOID(f != NULL);

   edd_log = _enlil_geocaching_log_edd_new();
   edd_tb = _enlil_geocaching_tb_edd_new();
   edd_gp = _enlil_geocaching_edd_new(edd_log, edd_tb);
   edd = _enlil_hashtable_edd_new(edd_gp);

   db = eet_data_read(f, edd, "/geocaching");

   enlil_file_manager_close(path);
   eet_data_descriptor_free(edd_log);
   eet_data_descriptor_free(edd_gp);
   eet_data_descriptor_free(edd);
   eet_data_descriptor_free(edd_tb);

   if(db)
   {
      _db.hash = db->hash;
      FREE(db);
   }
   else
     _db.hash = eina_hash_stringshared_new(EINA_FREE_CB(enlil_geocaching_free));
}

static int
_save(void)
{
   Eet_Data_Descriptor *edd, *edd_gp, *edd_log, *edd_tb;
   Eet_File *f;
   char path[PATH_MAX];
   int res;

   snprintf(path, PATH_MAX, "%s/" EET_FOLDER_ROOT_DB, getenv("HOME"));

   LOG_INFO("Save %s\n", path);

   if(!ecore_file_exists(path))
     ecore_file_mkpath(path);
   strcat(path, EET_FILE_GROUNDSPEAK_DB);

   f = enlil_file_manager_open(path);
   ASSERT_RETURN(f != NULL);

   edd_log = _enlil_geocaching_log_edd_new();
   edd_tb = _enlil_geocaching_tb_edd_new();
   edd_gp = _enlil_geocaching_edd_new(edd_log, edd_tb);
   edd = _enlil_hashtable_edd_new(edd_gp);

   res = eet_data_write(f, edd, "/geocaching", &_db, 0);

   enlil_file_manager_close(path);
   eet_data_descriptor_free(edd_log);
   eet_data_descriptor_free(edd_gp);
   eet_data_descriptor_free(edd);
   eet_data_descriptor_free(edd_tb);

   return res;
}

static void
_import_thread(void         *data,
               Ecore_Thread *thread __UNUSED__)
{
   const char *file = data;
   const char *value, *tag = NULL, *prefix = NULL;
   Enlil_Geocaching *gp = NULL, *gp2;
   Enlil_Geocaching_Log *gp_log = NULL;
   Enlil_Geocaching_Travelbug *gp_tb = NULL;
   Eina_Bool log = EINA_FALSE, tb = EINA_FALSE;

   const char *_wpt, *_name, *_time, *_description, *_url,
   *_url_name, *_sym, *_type,
   *_gp_name, *_gp_cache,
   *_gp_short_desc, *_gp_long_desc, *_gp_placed_by,
   *_gp_owner, *_gp_type, *_gp_container, *_gp_difficulty, *_gp_terrain,
   *_gp_country, *_gp_id, *_gp_available, *_gp_archived, *_gp_hints, *_gp_state, *_gp_owner_id,
   *_gp_log, *_gp_logs, *_gp_date, *_gp_finder, *_gp_text,
   *_gp_tbs, *_gp_tb;
   const char *pr_groundspeak;

   pr_groundspeak = eina_stringshare_add("groundspeak");

   _wpt = eina_stringshare_add("wpt");
   _name = eina_stringshare_add("name");
   _time = eina_stringshare_add("time");
   _description = eina_stringshare_add("desc");
   _url = eina_stringshare_add("url");
   _url_name = eina_stringshare_add("urlname");
   _sym = eina_stringshare_add("sym");
   _type = eina_stringshare_add("type");

   _gp_cache = eina_stringshare_add("cache");
   _gp_name = eina_stringshare_add("name");
   _gp_short_desc = eina_stringshare_add("short_description");
   _gp_long_desc = eina_stringshare_add("long_description");
   _gp_placed_by = eina_stringshare_add("placed_by");
   _gp_owner = eina_stringshare_add("owner");
   _gp_type = eina_stringshare_add("type");
   _gp_container = eina_stringshare_add("container");
   _gp_difficulty = eina_stringshare_add("difficulty");
   _gp_terrain = eina_stringshare_add("terrain");
   _gp_country = eina_stringshare_add("country");
   _gp_id = eina_stringshare_add("id");
   _gp_available = eina_stringshare_add("available");
   _gp_archived = eina_stringshare_add("archived");
   _gp_hints = eina_stringshare_add("encoded_hints");
   _gp_state = eina_stringshare_add("state");
   _gp_owner_id = eina_stringshare_add("owner_id");

   _gp_log = eina_stringshare_add("log");
   _gp_logs = eina_stringshare_add("logs");
   _gp_date = eina_stringshare_add("date");
   _gp_finder = eina_stringshare_add("finder");
   _gp_text = eina_stringshare_add("text");

   _gp_tbs = eina_stringshare_add("travelbugs");
   _gp_tb = eina_stringshare_add("travelbug");

   if(!_db.loaded)
     _load_thread(NULL, NULL);

   char *strip_ext = ecore_file_strip_ext(file);
   const char *ext = file + strlen(strip_ext);
   FREE(strip_ext);

   if(ext && !strcmp(ext, ".gpx"))
   {
      LOG_INFO("Import geocaching file %s", file);

      xmlDocPtr doc;
      xmlNodePtr cur;

      doc = xmlParseFile(file);

      if(doc)
      {
         cur = xmlDocGetRootElement(doc);

         XML_DOWN(cur);

         XML_GOTO(cur, "wpt", value);

         do
         {
            EINA_STRINGSHARE_DEL(tag);
            EINA_STRINGSHARE_DEL(prefix);
            tag = eina_stringshare_add((const char *)cur->name);
            if(cur->ns)
              prefix = eina_stringshare_add((const char *)cur->ns->prefix);

            if(tag)
            {
               if(!prefix && tag == _wpt)
               {
                  gp = enlil_geocaching_new();
                  xmlChar *s = xmlGetProp(cur, (xmlChar *)"lon");
                  if(s)
                  {
                     enlil_geocaching_longitude_set(gp, atof((const char *)s));
                     xmlFree(s);
                  }

                  s = xmlGetProp(cur, (xmlChar *)"lat");
                  if(s)
                  {
                     enlil_geocaching_latitude_set(gp, atof((const char *)s));
                     xmlFree(s);
                  }

                  XML_DOWN(cur);
               }
               else if(prefix == pr_groundspeak && tag == _gp_cache)
               {
                  xmlChar *s = xmlGetProp(cur, (xmlChar *)"id");
                  if(s)
                  {
                     enlil_geocaching_gp_id_set(gp, (const char *)s);
                     xmlFree(s);
                  }

                  s = xmlGetProp(cur, (xmlChar *)"available");
                  if(s)
                  {
                     enlil_geocaching_gp_available_set(gp, (const char *)s);
                     xmlFree(s);
                  }

                  s = xmlGetProp(cur, (xmlChar *)"archived");
                  if(s)
                  {
                     enlil_geocaching_gp_archived_set(gp, (const char *)s);
                     xmlFree(s);
                  }

                  XML_DOWN(cur);
               }
               else if(prefix == pr_groundspeak && tag == _gp_logs)
               {
                  XML_DOWN(cur);
               }
               else if(prefix == pr_groundspeak && tag == _gp_log)
               {
                  gp_log = enlil_geocaching_log_new();
                  Eina_List *l = enlil_geocaching_logs_get(gp);
                  l = eina_list_append(l, gp_log);
                  gp->logs = l;

                  xmlChar *s = xmlGetProp(cur, (xmlChar *)"id");
                  if(s)
                  {
                     enlil_geocaching_log_id_set(gp_log, (const char *)s);
                     xmlFree(s);
                  }

                  log = EINA_TRUE;
                  XML_DOWN(cur);
               }
               else if(prefix == pr_groundspeak && tag == _gp_tbs)
               {
                  XML_DOWN(cur);
               }
               else if(prefix == pr_groundspeak && tag == _gp_tb)
               {
                  gp_tb = enlil_geocaching_tb_new();
                  Eina_List *l = enlil_geocaching_tbs_get(gp);
                  l = eina_list_append(l, gp_tb);
                  gp->tbs = l;

                  xmlChar *s = xmlGetProp(cur, (xmlChar *)"id");
                  if(s)
                  {
                     enlil_geocaching_tb_id_set(gp_tb, (const char *)s);
                     xmlFree(s);
                  }

                  s = xmlGetProp(cur, (xmlChar *)"ref");
                  if(s)
                  {
                     enlil_geocaching_tb_ref_set(gp_tb, (const char *)s);
                     xmlFree(s);
                  }

                  tb = EINA_TRUE;
                  XML_DOWN(cur);
               }
               else
               {
                  if(!prefix && tag == _name)
                  {
                     xmlChar *xs = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                     enlil_geocaching_name_set(gp, (const char *)xs);
                     xmlFree(xs);

                     gp2 = eina_hash_find(_db.hash, enlil_geocaching_name_get(gp));
                     if(gp2)
                       eina_hash_del(_db.hash, enlil_geocaching_name_get(gp2), gp2);
                     eina_hash_direct_add(_db.hash, enlil_geocaching_name_get(gp), gp);
                  }
                  else if(!prefix && tag == _time)
                  {
                     xmlChar *xs = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                     enlil_geocaching_time_set(gp, (const char *)xs);
                     xmlFree(xs);
                  }
                  else if(!prefix && tag == _description)
                  {
                     xmlChar *xs = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                     enlil_geocaching_description_set(gp, (const char *)xs);
                     xmlFree(xs);
                  }
                  else if(!prefix && tag == _url)
                  {
                     xmlChar *xs = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                     enlil_geocaching_url_set(gp, (const char *)xs);
                     xmlFree(xs);
                  }
                  else if(!prefix && tag == _url_name)
                  {
                     xmlChar *xs = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                     enlil_geocaching_url_name_set(gp, (const char *)xs);
                     xmlFree(xs);
                  }
                  else if(!prefix && tag == _sym)
                  {
                     xmlChar *xs = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                     enlil_geocaching_sym_set(gp, (const char *)xs);
                     xmlFree(xs);
                  }
                  else if(!prefix && tag == _type)
                  {
                     xmlChar *xs = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                     enlil_geocaching_type_set(gp, (const char *)xs);
                     xmlFree(xs);
                  }
                  else if(prefix == pr_groundspeak && tag == _gp_name)
                  {
                     xmlChar *xs = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                     if(!tb)
                       enlil_geocaching_gp_name_set(gp, (const char *)xs);
                     else
                       enlil_geocaching_tb_name_set(gp_tb, (const char *)xs);
                     tb = EINA_FALSE;
                     xmlFree(xs);
                  }
                  else if(prefix == pr_groundspeak && tag == _gp_short_desc)
                  {
                     xmlChar *xs = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                     enlil_geocaching_gp_short_desc_set(gp, (const char *)xs);
                     xmlFree(xs);
                  }
                  else if(prefix == pr_groundspeak && tag == _gp_long_desc)
                  {
                     xmlChar *xs = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                     enlil_geocaching_gp_long_desc_set(gp, (const char *)xs);
                     xmlFree(xs);
                  }
                  else if(prefix == pr_groundspeak && tag == _gp_placed_by)
                  {
                     xmlChar *xs = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                     enlil_geocaching_gp_placed_by_set(gp, (const char *)xs);
                     xmlFree(xs);
                  }
                  else if(prefix == pr_groundspeak && tag == _gp_owner)
                  {
                     xmlChar *xs = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                     enlil_geocaching_gp_owner_set(gp, (const char *)xs);
                     xmlFree(xs);

                     xmlChar *s = xmlGetProp(cur, (xmlChar *)"id");
                     if(s)
                     {
                        enlil_geocaching_gp_owner_id_set(gp, (const char *)s);
                        xmlFree(s);
                     }
                  }
                  else if(prefix == pr_groundspeak && tag == _gp_type)
                  {
                     xmlChar *xs = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                     if(!log)
                       enlil_geocaching_gp_type_set(gp, (const char *)xs);
                     else
                       enlil_geocaching_log_type_set(gp_log, (const char *)xs);

                     log = EINA_FALSE;
                     xmlFree(xs);
                  }
                  else if(prefix == pr_groundspeak && tag == _gp_container)
                  {
                     xmlChar *xs = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                     enlil_geocaching_gp_container_set(gp, (const char *)xs);
                     xmlFree(xs);
                  }
                  else if(prefix == pr_groundspeak && tag == _gp_difficulty)
                  {
                     xmlChar *xs = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                     enlil_geocaching_gp_difficulty_set(gp, (const char *)xs);
                     xmlFree(xs);
                  }
                  else if(prefix == pr_groundspeak && tag == _gp_terrain)
                  {
                     xmlChar *xs = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                     enlil_geocaching_gp_terrain_set(gp, (const char *)xs);
                     xmlFree(xs);
                  }
                  else if(prefix == pr_groundspeak && tag == _gp_country)
                  {
                     xmlChar *xs = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                     enlil_geocaching_gp_country_set(gp, (const char *)xs);
                     xmlFree(xs);
                  }
                  else if(prefix == pr_groundspeak && tag == _gp_hints)
                  {
                     xmlChar *xs = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                     enlil_geocaching_gp_hints_set(gp, (const char *)xs);
                     xmlFree(xs);
                  }
                  else if(prefix == pr_groundspeak && tag == _gp_state)
                  {
                     xmlChar *xs = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                     enlil_geocaching_gp_state_set(gp, (const char *)xs);
                     xmlFree(xs);
                  }
                  else if(prefix == pr_groundspeak && tag == _gp_date)
                  {
                     xmlChar *xs = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                     enlil_geocaching_log_date_set(gp_log, (const char *)xs);
                     xmlFree(xs);
                  }
                  else if(prefix == pr_groundspeak && tag == _gp_text)
                  {
                     xmlChar *xs = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                     enlil_geocaching_log_text_set(gp_log, (const char *)xs);
                     xmlFree(xs);

                     xmlChar *s = xmlGetProp(cur, (xmlChar *)"encoded");
                     if(s)
                     {
                        enlil_geocaching_log_encoded_set(gp_log, (const char *)s);
                        xmlFree(s);
                     }
                  }
                  else if(prefix == pr_groundspeak && tag == _gp_finder)
                  {
                     xmlChar *xs = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                     enlil_geocaching_log_finder_set(gp_log, (const char *)xs);
                     xmlFree(xs);

                     xmlChar *s = xmlGetProp(cur, (xmlChar *)"id");
                     if(s)
                     {
                        enlil_geocaching_log_finder_id_set(gp_log, (const char *)s);
                        xmlFree(s);
                     }
                  }

                  if(cur && (!cur->next || !cur->next->next))
                  {
                     XML_UP(cur);

                     while(cur && (!cur->next || !cur->next->next))
                     {
                        XML_UP(cur);
                     }
                  }
                  else if(cur)
                    XML_NEXT(cur);
               }
            }
         } while(cur);

         EINA_STRINGSHARE_DEL(tag);
         EINA_STRINGSHARE_DEL(prefix);
         xmlFreeDoc(doc);
      }
   }

   eina_stringshare_del(file);

   _save();
}

static void
_import_end_cb(__UNUSED__ void         *data,
               __UNUSED__ Ecore_Thread *thread)
{
   running = EINA_FALSE;

   _db.loaded = EINA_TRUE;
   _db.done_cb(_db.data, _db.hash);
}

static Eet_Data_Descriptor *
_enlil_geocaching_edd_new(Eet_Data_Descriptor *edd_log,
                          Eet_Data_Descriptor *edd_tb)
{
   Eet_Data_Descriptor *edd;
   Eet_Data_Descriptor_Class eddc;

   ASSERT_RETURN(edd_log != NULL);
   ASSERT_RETURN(edd_tb != NULL);

   EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, Enlil_Geocaching);
   eddc.func.str_direct_alloc = NULL;
   eddc.func.str_direct_free = NULL;

   edd = eet_data_descriptor_file_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Geocaching, "name", name, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Geocaching, "time", time, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Geocaching, "description", description, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Geocaching, "url_name", url_name, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Geocaching, "url", url, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Geocaching, "sym", sym, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Geocaching, "type", type, EET_T_STRING);

   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Geocaching, "longitude", longitude, EET_T_DOUBLE);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Geocaching, "latitude", latitude, EET_T_DOUBLE);

   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Geocaching, "gp_name", gp_name, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Geocaching, "gp_short_desc", gp_short_desc, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Geocaching, "gp_long_desc", gp_long_desc, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Geocaching, "gp_placed_by", gp_placed_by, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Geocaching, "gp_owner", gp_owner, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Geocaching, "gp_type", gp_type, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Geocaching, "gp_container", gp_container, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Geocaching, "gp_difficulty", gp_difficulty, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Geocaching, "gp_terrain", gp_terrain, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Geocaching, "gp_country", gp_country, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Geocaching, "gp_id", gp_id, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Geocaching, "gp_available", gp_available, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Geocaching, "gp_archived", gp_archived, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Geocaching, "gp_hints", gp_hints, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Geocaching, "gp_state", gp_state, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Geocaching, "gp_owner_id", gp_owner_id, EET_T_STRING);

   EET_DATA_DESCRIPTOR_ADD_LIST(edd, Enlil_Geocaching, "logs", logs, edd_log);
   EET_DATA_DESCRIPTOR_ADD_LIST(edd, Enlil_Geocaching, "tbs", tbs, edd_tb);

   return edd;
}

static Eet_Data_Descriptor *
_enlil_hashtable_edd_new(Eet_Data_Descriptor *edd_gp)
{
   Eet_Data_Descriptor *edd;
   Eet_Data_Descriptor_Class eddc;

   ASSERT_RETURN(edd_gp != NULL);

   EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, Enlil_Geocaching_Db);
   eddc.func.str_direct_alloc = NULL;
   eddc.func.str_direct_free = NULL;

   edd = eet_data_descriptor_file_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_HASH(edd, Enlil_Geocaching_Db, "hash", hash, edd_gp);

   return edd;
}

static Eet_Data_Descriptor *
_enlil_geocaching_log_edd_new(void)
{
   Eet_Data_Descriptor *edd;
   Eet_Data_Descriptor_Class eddc;

   EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, Enlil_Geocaching_Log);
   eddc.func.str_direct_alloc = NULL;
   eddc.func.str_direct_free = NULL;

   edd = eet_data_descriptor_file_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Geocaching_Log, "id", id, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Geocaching_Log, "date", date, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Geocaching_Log, "type", type, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Geocaching_Log, "finder", finder, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Geocaching_Log, "finder_id", finder_id, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Geocaching_Log, "text", text, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Geocaching_Log, "encoded", encoded, EET_T_STRING);

   return edd;
}

static Eet_Data_Descriptor *
_enlil_geocaching_tb_edd_new(void)
{
   Eet_Data_Descriptor *edd;
   Eet_Data_Descriptor_Class eddc;

   EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, Enlil_Geocaching_Travelbug);
   eddc.func.str_direct_alloc = NULL;
   eddc.func.str_direct_free = NULL;

   edd = eet_data_descriptor_file_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Geocaching_Travelbug, "id", id, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Geocaching_Travelbug, "ref", ref, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Geocaching_Travelbug, "name", name, EET_T_STRING);

   return edd;
}

