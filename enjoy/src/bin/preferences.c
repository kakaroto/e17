#include "private.h"

typedef struct _Preferences          Preferences;
typedef struct _Preferences_Category Preferences_Category;

struct _Preferences
{
   Evas_Object *list;
   Eina_Hash *categories; /* name -> category */
   Eina_Hash *items; /* plugin -> plugin (set, check uniqueness) */
};

struct _Preferences_Category
{
   Elm_Genlist_Item *it;
   int items;
   char name[];
};

struct _Enjoy_Preferences_Plugin {
   const Enjoy_Preferences_Plugin_Api *api;
   Preferences_Category *cat; /* only if exists in UI */
   Elm_Genlist_Item *it; /* only if exists in UI */
   int priority;
};

static Preferences enjoy_preferences = {
  NULL, NULL, NULL
};
static Eina_List *pending_plugins = NULL;

static char *
preferences_itc_item_text_get(void *data, Evas_Object *obj __UNUSED__, const char *part __UNUSED__)
{
   Enjoy_Preferences_Plugin *p = data;
   return strdup(p->api->label_get(p));
}

static char *
preferences_itc_category_text_get(void *data, Evas_Object *obj __UNUSED__, const char *part __UNUSED__)
{
   Preferences_Category *pc = data;
   return strdup(pc->name);
}

static const Elm_Genlist_Item_Class preferences_itc_item = {
  "default",
  {
    preferences_itc_item_text_get,
    NULL,
    NULL,
    NULL
  }
};

static const Elm_Genlist_Item_Class preferences_itc_category = {
  "group_index",
  {
    preferences_itc_category_text_get,
    NULL,
    NULL,
    NULL
  }
};

static void
preferences_deleted(void *data, Evas *e __UNUSED__, Evas_Object *lst __UNUSED__, void *event_info __UNUSED__)
{
   Preferences *prefs = data;

   prefs->list = NULL;

   preferences_cover_clear_unregister();
   preferences_cover_local_search_unregister();
   preferences_db_clear_unregister();
   preferences_db_folder_add_unregister();
   preferences_db_optimize_unregister();
}

static int
preferences_category_cmp(const void *pa, const void *pb)
{
   const Elm_Genlist_Item *ia = pa, *ib = pb;
   const Preferences_Category *a = elm_genlist_item_data_get(ia);
   const Preferences_Category *b = elm_genlist_item_data_get(ib);
   return strcoll(a->name, b->name);
}

static int
preferences_item_cmp(const void *pa, const void *pb)
{
   const Elm_Genlist_Item *ia = pa, *ib = pb;
   const Enjoy_Preferences_Plugin *a = elm_genlist_item_data_get(ia);
   const Enjoy_Preferences_Plugin *b = elm_genlist_item_data_get(ib);
   int r = a->priority - b->priority;
   if (r)
     return r;
   return strcoll(a->api->label_get((Enjoy_Preferences_Plugin *)a),
                 b->api->label_get((Enjoy_Preferences_Plugin *)b));
}

static void
preferences_item_selected(void *data, Evas_Object *lst, void *event_info)
{
   Enjoy_Preferences_Plugin *p = data;
   Elm_Genlist_Item *it = event_info;
   Elm_Object_Item *oi;
   Evas_Object *naviframe = elm_object_parent_widget_get(lst);
   Evas_Object *prev_btn = NULL, *next_btn = NULL, *content = NULL;
   Eina_Bool old_auto_prev_btn, auto_prev_btn = EINA_TRUE;

   if (!p->api->activated(p, naviframe, &prev_btn, &next_btn,
                          &content, &auto_prev_btn))
     {
        ERR("Failed at activated() of plugin %p api %p", p, p->api);
        return;
     }

   elm_genlist_item_selected_set(it, EINA_FALSE);

   old_auto_prev_btn = elm_naviframe_prev_btn_auto_pushed_get(naviframe);
   elm_naviframe_prev_btn_auto_pushed_set(naviframe, auto_prev_btn);
   oi = elm_naviframe_item_push(naviframe, p->api->label_get(p),
                                prev_btn, next_btn, content, NULL);
   elm_object_item_part_text_set(oi, "subtitle", p->api->category_get(p));
   elm_naviframe_prev_btn_auto_pushed_set(naviframe, old_auto_prev_btn);
}

static Eina_Bool
preferences_item_add(Preferences *prefs, Enjoy_Preferences_Plugin *p)
{
   const char *catname;
   Preferences_Category *cat;

   if (eina_hash_find(prefs->items, &p))
     {
        ERR("Plugin already in preferences: %p", p);
        return EINA_FALSE;
     }

   catname = p->api->category_get(p);
   if ((!catname) || (catname[0] == '\0'))
     {
        ERR("plugin %p api %p category_get() returned nothing!", p, p->api);
        return EINA_FALSE;
     }

   cat = eina_hash_find(prefs->categories, catname);
   if (!cat)
     {
        size_t catnamelen = strlen(catname) + 1;
        cat = malloc(sizeof(*cat) + catnamelen);
        cat->items = 0;
        memcpy(cat->name, catname, catnamelen);
        eina_hash_add(prefs->categories, cat->name, cat);

        cat->it = elm_genlist_item_direct_sorted_insert
          (prefs->list, &preferences_itc_category, cat, NULL,
           ELM_GENLIST_ITEM_NONE, preferences_category_cmp, NULL, NULL);

        elm_genlist_item_display_only_set(cat->it, EINA_TRUE);
     }

   eina_hash_add(prefs->items, &p, p);

   cat->items++;
   p->cat = cat;
   p->it = elm_genlist_item_direct_sorted_insert
     (prefs->list, &preferences_itc_item, p, cat->it,
      ELM_GENLIST_ITEM_NONE, preferences_item_cmp,
      preferences_item_selected, p);

   DBG("plugin %p item %p cat %p (%s)", p, p->it, cat, cat->name);

   return EINA_TRUE;
}

static Eina_Bool
preferences_item_del(Preferences *prefs, Enjoy_Preferences_Plugin *p)
{
   if (!eina_hash_find(prefs->items, &p))
     {
        ERR("Could not find item for plugin %p", p);
        return EINA_FALSE;
     }

   if (!prefs->list)
     DBG("List already deleted, ignore item deletion");
   else
     elm_genlist_item_del(p->it);

   if (p->cat)
     {
        p->cat->items--;
        if (p->cat->items == 0)
          {
             if (prefs->list)
               elm_genlist_item_del(p->cat->it);
             eina_hash_del(prefs->categories, p->cat->name, p->cat);
             free(p->cat);
          }
     }

   eina_hash_del(prefs->items, &p, p);
   return EINA_TRUE;
}

static Evas_Object *
preferences_root_add(Preferences *prefs, Evas_Object *frame)
{
   Enjoy_Preferences_Plugin *p;

   prefs->list = elm_genlist_add(frame);

   EINA_LIST_FREE(pending_plugins, p)
     {
        if (!preferences_item_add(prefs, p))
          {
             ERR("Could not add plugin p %p api %p!", p, p->api);
             preferences_item_del(&enjoy_preferences, p);
             free(p);
          }
     }

   return prefs->list;
}

static void
preferences_freed(void *data, Evas *e __UNUSED__, Evas_Object *lst __UNUSED__, void *event_info __UNUSED__)
{
   Preferences *prefs = data;
   eina_hash_free(prefs->categories);
   prefs->categories = NULL;

   eina_hash_free(prefs->items);
   prefs->items = NULL;

   /* prefs is &enjoy_preferences. do not free */
}

Evas_Object *
preferences_add(Evas_Object *parent)
{
   Evas_Object *root, *frame = elm_naviframe_add(parent);
   static Eina_Bool first = EINA_TRUE;
   Preferences *prefs;

   prefs = &enjoy_preferences;
   if (!prefs->categories)
     prefs->categories = eina_hash_string_small_new(NULL);
   if (!prefs->items)
     prefs->items = eina_hash_pointer_new(NULL);

   evas_object_event_callback_add
     (frame, EVAS_CALLBACK_DEL, preferences_deleted, prefs);
   evas_object_event_callback_add
     (frame, EVAS_CALLBACK_FREE, preferences_freed, prefs);

   root = preferences_root_add(prefs, frame);
   if (!root)
     {
        evas_object_del(frame);
        return NULL;
     }

   if (first)
     {
        first = EINA_FALSE;
        preferences_cover_clear_register();
        preferences_cover_local_search_register();
        preferences_db_clear_register();
        preferences_db_folder_add_register();
        preferences_db_optimize_register();
     }

   elm_naviframe_item_push(frame, "Preferences", NULL, NULL, root, NULL);

   return frame;
}

EAPI Enjoy_Preferences_Plugin *
enjoy_preferences_plugin_register(const Enjoy_Preferences_Plugin_Api *api, int priority)
{
   Enjoy_Preferences_Plugin *p;
   if (!api)
     {
        ERR("Missing plugin api");
        return NULL;
     }
   if (api->version != ENJOY_PREFERENCES_PLUGIN_API_VERSION)
     {
        ERR("Invalid Enjoy_Preferences_Plugin_Api version: plugin=%u, enjoy=%u",
            api->version, ENJOY_PREFERENCES_PLUGIN_API_VERSION);
        return NULL;
     }
   if (!api->category_get)
     {
        ERR("plugin api=%p: api->category_get == NULL", api);
        return NULL;
     }
   if (!api->label_get)
     {
        ERR("plugin api=%p: api->label_get == NULL", api);
        return NULL;
     }
   if (!api->activated)
     {
        ERR("plugin api=%p: api->activated == NULL", api);
        return NULL;
     }

   p = calloc(1, sizeof(Enjoy_Preferences_Plugin));
   if (!p)
     {
        ERR("Could not allocate plugin structure");
        return NULL;
     }

   p->api = api;
   p->priority = priority;

   if (!enjoy_preferences.list)
     {
        DBG("plugin registered %p but pending (no GUI yet)", p);
        pending_plugins = eina_list_append(pending_plugins, p);
        return p;
     }

   if (!preferences_item_add(&enjoy_preferences, p))
     {
        ERR("Could not add plugin p %p api %p!", p, api);
        preferences_item_del(&enjoy_preferences, p);
        return NULL;
     }

   DBG("plugin registered %p", p);

   return p;
}

EAPI void
enjoy_preferences_plugin_unregister(Enjoy_Preferences_Plugin *p)
{
   if (!p)
     {
        ERR("No plugin given");
        return;
     }

   DBG("plugin unregistered %p", p);

   if (p->cat)
     preferences_item_del(&enjoy_preferences, p);
   else
     pending_plugins = eina_list_remove(pending_plugins, p);

   free(p);
}
