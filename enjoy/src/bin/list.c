#include "private.h"

typedef struct _List
{
   DB *db;
   Evas_Object *pager;
   struct {
      Eina_List *list;
      Evas_Object *current;
      Evas_Object *songs;
   } page;
} List;

static List _list;

#define LIST_GET_OR_RETURN(list, obj, ...)                      \
  List *list = evas_object_data_get(obj, "_enjoy_list");        \
  do { if (!list) return __VA_ARGS__; } while (0)


static void
_list_page_back(void *data, Evas_Object *o, void *event_info __UNUSED__)
{
   List *list = data;
   if (list->page.songs == o) list->page.songs = NULL;
   list->page.list = eina_list_remove(list->page.list, o);
   evas_object_del(o);
   if (list->page.current == o)
     list->page.current = elm_pager_content_top_get(list->pager);
}

static void
_list_page_song(void *data, Evas_Object *o __UNUSED__, void *event_info)
{
   List *list = data;
   Song *song = event_info;
   evas_object_smart_callback_call(list->pager, "selected", song);
}

static void
_list_page_folder(void *data, Evas_Object *o __UNUSED__, void *event_info)
{
   List *list = data;
   Evas_Object *page = event_info;
   EINA_SAFETY_ON_NULL_RETURN(page);
   list->page.current = page;
   evas_object_smart_callback_add(page, "back", _list_page_back, list);
   evas_object_smart_callback_add(page, "song", _list_page_song, list);
   evas_object_smart_callback_add(page, "folder", _list_page_folder, list);
   list->page.list = eina_list_append(list->page.list, page);
   elm_pager_content_push(list->pager, page);
}

static void
_list_del(void *data, Evas *e __UNUSED__, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   List *list = data;
   eina_list_free(list->page.list);
}

Evas_Object *
list_add(Evas_Object *parent)
{
   List *list = &_list;

   memset(list, 0, sizeof(list));

   list->pager = elm_pager_add(parent);
   if (!list->pager) return NULL;

   evas_object_data_set(list->pager, "_enjoy_list", list);
   evas_object_event_callback_add
     (list->pager, EVAS_CALLBACK_DEL, _list_del, list);

   return list->pager;
}

Eina_Bool
list_populate(Evas_Object *obj, DB *db)
{
   LIST_GET_OR_RETURN(list, obj, EINA_FALSE);
   Evas_Object *page;

   EINA_SAFETY_ON_NULL_RETURN_VAL(list, EINA_FALSE);
   EINA_LIST_FREE(list->page.list, page) evas_object_del(page);
   list->page.current = list->page.songs = NULL;
   list->db = db;
   if (!db) return EINA_TRUE;

   page = list->page.current = page_root_add(obj);
   if (!page) return EINA_FALSE;
   evas_object_smart_callback_add(page, "folder", _list_page_folder, list);
   list->page.list = eina_list_append(list->page.list, page);
   elm_pager_content_push(list->pager, page);

   return EINA_TRUE;
}

Song *
list_selected_get(const Evas_Object *obj)
{
   LIST_GET_OR_RETURN(list, obj, NULL);
   if (list->page.songs) return page_songs_selected_get(list->page.songs);
   return NULL;
}

Eina_Bool
list_next_exists(const Evas_Object *obj)
{
   LIST_GET_OR_RETURN(list, obj, EINA_FALSE);
   if (list->page.songs) return page_songs_next_exists(list->page.songs);
   return EINA_FALSE;
}

Song *
list_next_go(Evas_Object *obj)
{
   LIST_GET_OR_RETURN(list, obj, NULL);
   if (list->page.songs) return page_songs_next_go(list->page.songs);
   return NULL;
}

Eina_Bool
list_prev_exists(const Evas_Object *obj)
{
   LIST_GET_OR_RETURN(list, obj, EINA_FALSE);
   if (list->page.songs) return page_songs_prev_exists(list->page.songs);
   return EINA_FALSE;
}

Song *
list_prev_go(Evas_Object *obj)
{
   LIST_GET_OR_RETURN(list, obj, NULL);
   if (list->page.songs) return page_songs_prev_go(list->page.songs);
   return NULL;
}

DB *
list_db_get(const Evas_Object *obj)
{
   LIST_GET_OR_RETURN(list, obj, NULL);
   return list->db;
}
