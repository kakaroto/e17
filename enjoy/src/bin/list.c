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
   unsigned int frozen;
} List;

static List _list;

#define LIST_GET_OR_RETURN(list, obj, ...)                      \
  List *list = evas_object_data_get(obj, "_enjoy_list");        \
  do { if (!list) return __VA_ARGS__; } while (0)


static void
_list_page_promote(List *list, Evas_Object *page)
{
   DBG("page=%p (%s), current=%p (%s)",
       page, page_title_get(page),
       list->page.current,
       list->page.current ? page_title_get(list->page.current) : NULL);

   if (list->page.current == page) return;
   list->page.current = page;
   elm_pager_content_promote(list->pager, page);
}

static void
_list_page_playing(void *data, Evas_Object *o __UNUSED__, void *event_info)
{
   List *list = data;
   DBG("show songs folder %p (%s) requested by %p (%s)",
       list->page.songs, page_title_get(list->page.songs),
       event_info, page_title_get(event_info));

   if (list->page.songs == data) return;
   if (!list->page.songs) return;

   EINA_SAFETY_ON_NULL_RETURN(eina_list_last(list->page.list));
   if (eina_list_last(list->page.list)->data != list->page.songs)
        list->page.list = eina_list_append(list->page.list, list->page.songs);

   _list_page_promote(list, list->page.songs);
   page_playing_hide(list->page.current);
   page_back_show(list->page.current);
}

static void
_list_page_remove(List *list, Evas_Object *page)
{
   Eina_List *l;

   DBG("page=%p (%s), songs=%p (%s), current=%p (%s)",
       page, page_title_get(page),
       list->page.songs,
       list->page.songs ? page_title_get(list->page.songs) : "",
       list->page.current,
       list->page.current ? page_title_get(list->page.current): "");

   list->page.list = eina_list_remove(list->page.list, page);
   if (list->page.songs != page) evas_object_del(page);

   if (list->page.current != page) return;
   list->page.current = NULL;
   l = eina_list_last(list->page.list);
   EINA_SAFETY_ON_NULL_RETURN(l);
   _list_page_promote(list, l->data);
}

static Eina_Bool
_list_page_add(List *list, Evas_Object *page)
{
   DBG("page=%p (%s)", page, page_title_get(page));
   EINA_SAFETY_ON_NULL_RETURN_VAL(page, EINA_FALSE);
   list->page.list = eina_list_append(list->page.list, page);
   list->page.current = page;
   elm_pager_content_push(list->pager, page);
   if (eina_list_count(list->page.list) > 1)
      page_back_show(list->page.current);
   if ((list->page.songs) && (list->page.songs != page))
      page_playing_show(list->page.current);
   else
      page_playing_hide(list->page.current);

   return EINA_TRUE;
}

static void
_list_page_back(void *data, Evas_Object *o __UNUSED__, void *event_info)
{
   List *list = data;
   Evas_Object *page = event_info;
   DBG("page=%p (%s)", page, page_title_get(page));
   _list_page_remove(list, list->page.current);
   if (eina_list_count(list->page.list) == 1)
      page_back_hide(list->page.current);
   if ((list->page.songs) && (list->page.songs != list->page.current))
      page_playing_show(list->page.current);
   else
      page_playing_hide(list->page.current);
}

static void
_list_page_song(void *data, Evas_Object *page, void *event_info)
{
   List *list = data;
   Song *song = event_info;

   if (list->page.songs != page)
     {
        Evas_Object *o;

        if (list->page.songs)
          {
             o = list->page.songs;
             list->page.songs = NULL; /* force o being deleted */
             _list_page_remove(list, o);
          }

        list->page.songs = page;
        page_playing_hide(page);
     }

   evas_object_smart_callback_call(list->pager, "selected", song);
}

static void
_list_page_folder_songs(void *data, Evas_Object *o __UNUSED__, void *event_info)
{
   List *list = data;
   Evas_Object *page = event_info;
   EINA_SAFETY_ON_NULL_RETURN(page);
   if (!_list_page_add(list, page)) return;
   evas_object_smart_callback_add(page, "song", _list_page_song, list);
   evas_object_smart_callback_add(page, "back", _list_page_back, list);
   evas_object_smart_callback_add(page, "playing", _list_page_playing, list);
}

static void
_list_page_folder(void *data, Evas_Object *o __UNUSED__, void *event_info)
{
   List *list = data;
   Evas_Object *page = event_info;
   EINA_SAFETY_ON_NULL_RETURN(page);
   if (!_list_page_add(list, page)) return;
   evas_object_smart_callback_add(page, "folder", _list_page_folder, list);
   evas_object_smart_callback_add
     (page, "folder-songs", _list_page_folder_songs, list);
   evas_object_smart_callback_add(page, "back", _list_page_back, list);
   evas_object_smart_callback_add(page, "playing", _list_page_playing, list);
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
   elm_object_style_set(list->pager, "fade");
   if (!list->pager) return NULL;

   evas_object_data_set(list->pager, "_enjoy_list", list);
   evas_object_event_callback_add
     (list->pager, EVAS_CALLBACK_DEL, _list_del, list);

   return list->pager;
}

void
list_promote_current(Evas_Object *obj)
{
   LIST_GET_OR_RETURN(list, obj);
   elm_pager_content_promote(obj, list->page.current);
}

Eina_Bool
list_populate(Evas_Object *obj, DB *db)
{
   LIST_GET_OR_RETURN(list, obj, EINA_FALSE);
   Evas_Object *page;

   if (list->frozen)
     return EINA_FALSE;

   EINA_SAFETY_ON_NULL_RETURN_VAL(list, EINA_FALSE);
   EINA_LIST_FREE(list->page.list, page) evas_object_del(page);
   list->page.current = list->page.songs = NULL;
   list->db = db;
   if (!db) return EINA_TRUE;

   page = page_root_add(obj);

   EINA_SAFETY_ON_NULL_RETURN_VAL(page, EINA_FALSE);
   if (!_list_page_add(list, page)) return EINA_FALSE;
   evas_object_smart_callback_add(page, "folder", _list_page_folder, list);
   evas_object_smart_callback_add
     (page, "folder-songs", _list_page_folder_songs, list);
   evas_object_smart_callback_add(page, "back", _list_page_back, list);
   evas_object_smart_callback_add(page, "playing", _list_page_playing, list);
   page_back_hide(list->page.current);

   return EINA_TRUE;
}

Eina_Bool
list_songs_exists(const Evas_Object *obj)
{
   LIST_GET_OR_RETURN(list, obj, EINA_FALSE);
   return !!list->page.songs;
}

Song *
list_selected_get(const Evas_Object *obj)
{
   LIST_GET_OR_RETURN(list, obj, NULL);
   if (list->page.songs) return page_songs_selected_get(list->page.songs);
   return NULL;
}

Eina_Bool
list_song_updated(Evas_Object *obj)
{
   LIST_GET_OR_RETURN(list, obj, EINA_FALSE);
   if (list->page.songs) return page_songs_song_updated(list->page.songs);
   return EINA_FALSE;
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

int32_t
list_song_selected_n_get(const Evas_Object *obj)
{
   LIST_GET_OR_RETURN(list, obj, 0);
   if (list->page.songs) return page_songs_selected_n_get(list->page.songs);
   return 0;
}

int32_t
list_song_count(const Evas_Object *obj)
{
   LIST_GET_OR_RETURN(list, obj, 0);
   if (list->page.songs) return page_songs_count(list->page.songs);
   return 0;
}

Song *
list_song_nth_get(const Evas_Object *obj, int32_t n)
{
   LIST_GET_OR_RETURN(list, obj, NULL);
   if (list->page.songs) return page_songs_nth_get(list->page.songs, n);
   return NULL;
}

Song *
list_shuffle_next_go(Evas_Object *obj)
{
   LIST_GET_OR_RETURN(list, obj, NULL);
   if (list->page.songs) return page_songs_shuffle_next_go(list->page.songs);
   return NULL;
}

void
list_shuffle_reset(Evas_Object *obj)
{
   LIST_GET_OR_RETURN(list, obj);
   if (list->page.songs) return page_songs_shuffle_reset(list->page.songs);
}

Song *
list_shuffle_prev_go(Evas_Object *obj)
{
   LIST_GET_OR_RETURN(list, obj, NULL);
   if (list->page.songs) return page_songs_shuffle_prev_go(list->page.songs);
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

void
list_freeze(Evas_Object *obj)
{
   LIST_GET_OR_RETURN(list, obj);
   list->frozen++;
}

void
list_thaw(Evas_Object *obj)
{
   LIST_GET_OR_RETURN(list, obj);
   list->frozen--;
}
