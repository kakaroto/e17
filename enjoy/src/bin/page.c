#include "private.h"
#include "mpris.h"

#include <ctype.h>
#include <stdlib.h>

/*
 * TODO:
 *  - create page_folder that contains something other than songs, click one
 *    should create a new page, stack it into the parent list and recurse.
 *  - add suffle action for page_songs
 */

/* number of songs to populate at once before going back to mainloop */
#define PAGE_SONGS_POPULATE_ITERATION_COUNT (64)

/* number of folders to populate at once before going back to mainloop */
#define PAGE_FOLDERS_POPULATE_ITERATION_COUNT (64)

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

struct Array_Iterator
{
   Eina_Iterator base;
   const void *container;
   size_t current, count, item_size;
};

static void *
_array_iterator_container_get(Eina_Iterator *iterator)
{
   struct Array_Iterator *it = (struct Array_Iterator *)iterator;
   if (!EINA_MAGIC_CHECK(iterator, EINA_MAGIC_ITERATOR))
     {
        EINA_MAGIC_FAIL(iterator, EINA_MAGIC_ITERATOR);
        return NULL;
     }
   return (void *)it->container;
}

static void
_array_iterator_free(Eina_Iterator *iterator)
{
   struct Array_Iterator *it = (struct Array_Iterator *)iterator;
   if (!EINA_MAGIC_CHECK(iterator, EINA_MAGIC_ITERATOR))
     {
        EINA_MAGIC_FAIL(iterator, EINA_MAGIC_ITERATOR);
        return;
     }
   EINA_MAGIC_SET(&it->base, EINA_MAGIC_NONE);
   free(it);
}

static Eina_Bool
_array_iterator_next(Eina_Iterator *iterator, void **data)
{
   struct Array_Iterator *it = (struct Array_Iterator *)iterator;
   if (!EINA_MAGIC_CHECK(iterator, EINA_MAGIC_ITERATOR))
     {
        EINA_MAGIC_FAIL(iterator, EINA_MAGIC_ITERATOR);
        return EINA_FALSE;
     }
   EINA_SAFETY_ON_NULL_RETURN_VAL(data, EINA_FALSE);
   *data = NULL;
   if (it->current >= it->count) return EINA_FALSE;
   *data = ((char *)it->container) + it->item_size * it->current;
   it->current++;
   return EINA_TRUE;
}

static Eina_Iterator *
_array_iterator_new(const void *array, size_t item_size, size_t count)
{
   struct Array_Iterator *it = calloc(1, sizeof(*it));
   EINA_SAFETY_ON_NULL_RETURN_VAL(it, NULL);
   it->base.get_container = _array_iterator_container_get;
   it->base.free = _array_iterator_free;
   it->base.next = _array_iterator_next;
   it->container = array;
   it->item_size = item_size;
   it->count = count;
   EINA_MAGIC_SET(&it->base, EINA_MAGIC_ITERATOR);
   return &it->base;
}

static void *
_data_from_itr_passthrough(const void *data)
{
   return (void *)data;
}

typedef struct _Page Page;

typedef struct _Page_Class
{
   const char *name;
   const char *key;
   const char *layout;
   Eina_Bool (*init)(Page *page);      /* optional extra initializer */
   void (*after_populate)(Page *page); /* called after page has been populated */
   Evas_Smart_Cb selected;
   const Elm_Genlist_Item_Class *item_cls;
   size_t populate_iteration_count;
   void *(*data_from_itr)(const void *data);
   size_t data_letter_offset;
   unsigned short icon_size;
} Page_Class;

struct _Page
{
   const Page_Class *cls;
   Evas_Object *layout_list;
   Evas_Object *layout;
   Evas_Object *edje;
   Evas_Object *edje_list;
   Evas_Object *list;
   Evas_Object *index;
   Evas_Object *parent;
   size_t num_elements;
   Eina_Array *shuffle;
   size_t shuffle_position;
   const char *title;
   void *container;
   void *model;
   Elm_Genlist_Item *selected;
   Elm_Genlist_Item *first;
   Eina_Iterator *iterator;
   Ecore_Idler *populate;
   Eina_Hash *od_to_list_item;
   char last_index_letter[2];
   struct {
      void *data;
      void (*free)(void *data);
   } item;
};

#define PAGE_GET_OR_RETURN(page, obj, ...)                      \
  Page *page = evas_object_data_get(obj, "_enjoy_page");        \
  do                                                            \
    {                                                           \
       if (!page)                                               \
         {                                                      \
            CRITICAL("Not a page: "#obj": %p", obj);            \
            return __VA_ARGS__;                                 \
         }                                                      \
    }                                                           \
  while (0)

static DB *
_page_db_get(const Evas_Object *obj)
{
   Page *page, *ppage;

   page = evas_object_data_get(obj, "_enjoy_page");
   EINA_SAFETY_ON_NULL_RETURN_VAL(page, NULL);
   ppage = evas_object_data_get(page->parent, "_enjoy_page");
   if (ppage) return _page_db_get(page->parent);
   return list_db_get(page->parent);
}

static Eina_Bool
_page_populate(void *data)
{
   Page *page = data;
   const Page_Class *cls = page->cls;
   unsigned int count;

   page->od_to_list_item = eina_hash_pointer_new(NULL);

   for (count = 0; count < cls->populate_iteration_count; count++)
     {
        Elm_Genlist_Item *it;
        char letter;
        void *id, *od;
        const char **letter_str;

        if (!eina_iterator_next(page->iterator, &id)) goto end;
        // TODO: evaluate if we should keep a full copy or just store
        // fields of interest such as id, title, artist and album
        od = cls->data_from_itr(id);
        if (!od) goto end;

        it = elm_genlist_item_append
          (page->list, cls->item_cls, od,
           NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);

        letter_str = (const char **)(((char *)od) + cls->data_letter_offset);

        letter = toupper((*letter_str)[0]);
        if ((page->index) &&
            (isalpha(letter) && (page->last_index_letter[0] != letter)))
          {
             if ((page->first) && (!page->last_index_letter[0]))
               elm_index_item_append(page->index, "Special", page->first);

             page->last_index_letter[0] = letter;
             elm_index_item_append(page->index, page->last_index_letter, it);
          }
        if (!page->first) page->first = it;
        eina_hash_set(page->od_to_list_item, od, it);
        page->num_elements++;
     }

   return EINA_TRUE;

 end:
   if (cls->after_populate)
     cls->after_populate(page);

   page->populate = NULL;
   return EINA_FALSE;
}

static void
_page_selected(void *data, Evas_Object *o, void *event_info)
{
   Page *page = data;
   Elm_Genlist_Item *it = event_info;
   if (page->selected == it) return;
   page->selected = it;
   page->cls->selected(data, o, event_info);
}

static void
_page_index_changed(void *data __UNUSED__, Evas_Object *o __UNUSED__, void *event_info)
{
   Elm_Genlist_Item *it = event_info;
   elm_genlist_item_top_bring_in(it);
}

static void
_page_del(void *data, Evas *e __UNUSED__, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Page *page = data;
   eina_stringshare_del(page->title);
   if (page->iterator) eina_iterator_free(page->iterator);
   if (page->populate) ecore_idler_del(page->populate);
   if (page->item.free) page->item.free(page->item.data);
   if (page->od_to_list_item) eina_hash_free(page->od_to_list_item);
   free(page);
}

static void
_page_action_back(void *data, Evas_Object *obj __UNUSED__, const char *emission __UNUSED__, const char *source __UNUSED__)
{
   Page *page = data;
   evas_object_smart_callback_call(page->layout, "back", page->layout);
}

static void
_page_action_next(void *data, Evas_Object *obj __UNUSED__, const char *emission __UNUSED__, const char *source __UNUSED__)
{
   Page *page = data;
   evas_object_smart_callback_call(page->layout, "playing", page->layout);
}

static Evas_Object *
_page_add(Evas_Object *parent, void *model, Eina_Iterator *it, const char *title, const Page_Class *cls)
{
   Evas_Object *obj_list, *obj = NULL;
   Page *page;
   const char *s;
   Edje_External_Param param;

   EINA_SAFETY_ON_NULL_RETURN_VAL(it, NULL);
   EINA_SAFETY_ON_NULL_RETURN_VAL(cls, NULL);
   EINA_SAFETY_ON_NULL_RETURN_VAL(cls->name, NULL);
   EINA_SAFETY_ON_NULL_RETURN_VAL(cls->key, NULL);
   EINA_SAFETY_ON_NULL_RETURN_VAL(cls->layout, NULL);
   EINA_SAFETY_ON_NULL_RETURN_VAL(cls->selected, NULL);
   EINA_SAFETY_ON_NULL_RETURN_VAL(cls->item_cls, NULL);
   EINA_SAFETY_ON_NULL_RETURN_VAL(cls->data_from_itr, NULL);

   DBG("creating page %s with key %s, item style %s",
       cls->name, cls->key, cls->item_cls->item_style);

   obj_list = elm_layout_add(parent);
   if (!obj_list)
     {
        eina_iterator_free(it);
        return NULL;
     }
   obj = elm_layout_add(parent);
   if (!obj)
     {
        eina_iterator_free(it);
        goto error_layout;
     }

   page = calloc(1, sizeof(*page));
   if (!page)
     {
        CRITICAL("could not allocate page memory!");
        eina_iterator_free(it);
        goto error;
     }
   evas_object_data_set(obj, "_enjoy_page", page);
   evas_object_data_set(obj, cls->key, page);
   evas_object_event_callback_add(obj_list, EVAS_CALLBACK_DEL, _page_del, page);
   page->layout_list = obj_list;
   page->model = model;
   page->iterator = it;
   page->cls = cls;
   page->parent = parent;
   page->od_to_list_item = NULL;
   page->layout = obj;

   if (!elm_layout_theme_set(obj_list, "layout", "enjoy", cls->layout))
     {
        if (!elm_layout_file_set(obj_list,
                                 PACKAGE_DATA_DIR "/default.edj", cls->layout))
          {
             CRITICAL("no theme for '%s' at %s",
                      cls->layout, PACKAGE_DATA_DIR "/default.edj");
             goto error;
          }
     }

   if (!elm_layout_theme_set
       (obj, "layout", "application", "content-back-next"))
     {
        CRITICAL("no theme for 'elm/layout/application/content-back-next'.");
        goto error;
     }

   page->title = eina_stringshare_add(title);
   page->edje_list = elm_layout_edje_get(obj_list);

   page->list = elm_genlist_add(obj_list);
   elm_genlist_bounce_set(page->list, EINA_FALSE, EINA_TRUE);
   elm_genlist_horizontal_mode_set(page->list, ELM_LIST_COMPRESS);
   elm_genlist_compress_mode_set(page->list, EINA_TRUE);

   s = edje_object_data_get(page->edje_list, "homogeneous");
   elm_genlist_homogeneous_set(page->list, s ? !!atoi(s) : EINA_FALSE);

   elm_layout_content_set(obj_list, "ejy.swallow.list", page->list);

   if (edje_object_part_exists(page->edje_list, "ejy.swallow.index"))
     {
        page->index = elm_index_add(obj_list);
        evas_object_smart_callback_add
          (page->index, "delay,changed", _page_index_changed, page);
        elm_layout_content_set(obj_list, "ejy.swallow.index", page->index);
     }

   page->edje = elm_layout_edje_get(page->layout);
   elm_layout_content_set(page->layout, "elm.swallow.content", page->layout_list);
   edje_object_part_text_set(page->edje,
                             "elm.text.title", page->title);
   edje_object_signal_callback_add(page->edje, "elm,action,back", "",
                                   _page_action_back, page);
   edje_object_signal_callback_add(page->edje, "elm,action,next", "",
                                   _page_action_next, page);
   param.type = EDJE_EXTERNAL_PARAM_TYPE_STRING;
   param.name = "label";
   param.s = "Playing";
   edje_object_part_external_param_set (page->edje, "next", &param);


   page->container = eina_iterator_container_get(it);
   evas_object_data_set(page->list, "_enjoy_container", page->container);
   evas_object_data_set(page->list, "_enjoy_page", page);
   evas_object_smart_callback_add(page->list, "selected", _page_selected, page);

   if (cls->init)
     {
        if (!cls->init(page)) goto error;
     }

   page->populate = ecore_idler_add(_page_populate, page);

   return obj;

error:
   evas_object_del(obj);
error_layout:
   evas_object_del(obj_list);
   return NULL;
}

const char *
page_title_get(const Evas_Object *obj)
{
   PAGE_GET_OR_RETURN(page, obj, NULL);
   return page->title;
}


/***********************************************************************
 * SONGS
 ***********************************************************************/

#define PAGE_SONGS_GET_OR_RETURN(page, obj, ...)                \
  Page *page = evas_object_data_get(obj, "_enjoy_page_songs");  \
  do                                                            \
    {                                                           \
       if (!page)                                               \
         {                                                      \
            CRITICAL("Not a page_song: "#obj": %p", obj);       \
            return __VA_ARGS__;                                 \
         }                                                      \
    }                                                           \
  while (0)


static char *
_song_item_label_get(void *data, Evas_Object *list, const char *part)
{
   Song *song = data;

   /* check if matches protocol */
   if (strncmp(part, "elm.text.", sizeof("elm.text.") - 1) != 0)
     return NULL;
   part += sizeof("elm.text.") - 1;

   if (!strcmp(part, "title"))
     return strdup(song->title);
   else if (!strcmp(part, "trackno-title"))
     {
        char *str;
        if (song->trackno < 1) return strdup(song->title);
        if (asprintf(&str, "%d - %s", song->trackno, song->title) > 0)
          return str;
        return NULL;
     }
   else if (!strcmp(part, "album-artist"))
     {
        char *str;

        if ((!song->flags.fetched_album) || (!song->flags.fetched_artist))
          {
             DB *db = evas_object_data_get(list, "_enjoy_container");
             db_song_album_fetch(db, song);
             db_song_artist_fetch(db, song);
          }

        if ((!song->album) && (!song->artist)) return NULL;
        else if (!song->album) return strdup(song->artist);
        else if (!song->artist) return strdup(song->album);

        if (asprintf(&str, "%s - %s", song->album, song->artist) > 0)
          return str;
        return NULL;
     }
   else if (!strcmp(part, "album"))
     {
        if (!song->flags.fetched_album)
          {
             DB *db = evas_object_data_get(list, "_enjoy_container");
             db_song_album_fetch(db, song);
          }
        return song->album ? strdup(song->album) : NULL;
     }
   else if (!strcmp(part, "artist"))
     {
        if (!song->flags.fetched_artist)
          {
             DB *db = evas_object_data_get(list, "_enjoy_container");
             db_song_artist_fetch(db, song);
          }
        return song->artist ? strdup(song->artist) : NULL;
     }
   else if (!strcmp(part, "genre"))
     {
        if (!song->flags.fetched_genre)
          {
             DB *db = evas_object_data_get(list, "_enjoy_container");
             db_song_genre_fetch(db, song);
          }
        return song->genre ? strdup(song->genre) : NULL;
     }
   else if (!strcmp(part, "trackno"))
     {
        char *str;
        if (song->trackno < 1) return NULL;
        if (asprintf(&str, "%d", song->trackno) > 0)
          return str;
        return NULL;
     }
   else if (!strcmp(part, "playcnt"))
     {
        char *str;
        if (song->playcnt < 1) return NULL;
        if (asprintf(&str, "%d", song->playcnt) > 0)
          return str;
        return NULL;
     }
   else if (!strcmp(part, "rating"))
     {
        char *str;
        if (song->rating < 1) return NULL;
        if (asprintf(&str, "%d", song->rating) > 0)
          return str;
        return NULL;
     }
   else if (!strcmp(part, "length"))
     {
        char *str;
        int len;
        if (song->length < 1) return NULL;
        if (song->length < 60)
          len = asprintf(&str, "%d", song->length);
        else if (song->length < 60 * 60)
          len = asprintf(&str, "%d:%02d", song->length / 60, song->length % 60);
        else
          len = asprintf(&str, "%d:%02d:%02d",
                         song->length / (60 * 60),
                         (song->length / 60) % 60,
                         song->length % 60);
        if (len > 0) return str;
        return NULL;
     }

   return NULL;
}

static Eina_Bool
_song_item_state_get(void *data, Evas_Object *list, const char *part)
{
   Song *song = data;

   /* check if matches protocol */
   if (strncmp(part, "elm.state.", sizeof("elm.state.") - 1) != 0)
     return EINA_FALSE;
   part += sizeof("elm.state.") - 1;

   if (!strcmp(part, "title")) return EINA_TRUE;
   else if (!strcmp(part, "trackno")) return song->trackno > 0;
   else if (!strcmp(part, "playcnt")) return song->playcnt > 0;
   else if (!strcmp(part, "rating")) return song->rating > 0;
   else if (!strcmp(part, "length")) return song->length > 0;
   else if (!strcmp(part, "artist"))
     {
        if (!song->flags.fetched_artist)
          {
             DB *db = evas_object_data_get(list, "_enjoy_container");
             db_song_artist_fetch(db, song);
          }

        return !!song->artist;
     }
   else if (!strcmp(part, "album"))
     {
        if (!song->flags.fetched_album)
          {
             DB *db = evas_object_data_get(list, "_enjoy_container");
             db_song_album_fetch(db, song);
          }

        return !!song->album;
     }
   else if (!strcmp(part, "genre"))
     {
        if (!song->flags.fetched_genre)
          {
             DB *db = evas_object_data_get(list, "_enjoy_container");
             db_song_genre_fetch(db, song);
          }
        return !!song->genre;
     }

   return EINA_FALSE;
}


static void
_song_item_del(void *data, Evas_Object *list __UNUSED__)
{
   db_song_free(data);
}

static void
_song_item_selected(void *data, Evas_Object *list __UNUSED__, void *event_info)
{
   Page *page = data;
   Song *song = elm_genlist_item_data_get(event_info);
   if (song) evas_object_smart_callback_call(page->layout, "song", song);
}

static void
_page_songs_after_populate(Page *page)
{
   mpris_signal_tracklist_tracklist_change(page->num_elements);
}

static Evas_Object *
_page_songs_add(Evas_Object *parent, NameID *nameid, Eina_Iterator *it, const char *title)
{
   static const Elm_Genlist_Item_Class song_item_cls = {
     "media",
     {
       _song_item_label_get,
       NULL,
       _song_item_state_get,
       _song_item_del
     }
   };
   static const Page_Class song_cls = {
     "song",
     "_enjoy_page_songs",
     "page/songs",
     NULL,
     _page_songs_after_populate,
     _song_item_selected,
     &song_item_cls,
     PAGE_SONGS_POPULATE_ITERATION_COUNT,
     (void *(*)(const void*))db_song_copy,
     offsetof(Song, title),
     0
   };
   return _page_add(parent, nameid, it, title, &song_cls);
}

static void
_song_album_cover_size_changed(void *data, Evas *e __UNUSED__, Evas_Object *part, void *event_info __UNUSED__)
{
   Page *page = data;
   Evas_Coord size, w, h;
   Evas_Object *cover;

   evas_object_geometry_get(part, NULL, NULL, &w, &h);
   if (w < 32) w = 32;
   if (h < 32) h = 32;
   size = (w < h) ? w : h;

   DBG("cover view changed size to %dx%d, query cover size %d", w, h, size);
   cover = cover_album_fetch(page->layout, page->container, page->model, size, NULL, NULL);
   elm_layout_content_set(page->layout_list, "ejy.swallow.cover", cover);
}

static Eina_Bool
_song_album_init(Page *page)
{
   Album *album = page->model;

   if (edje_object_part_exists(page->edje_list, "ejy.swallow.cover"))
     {
        DB *db = _page_db_get(page->layout);
        Evas_Object *cover, *part;
        const char *s;
        int size = 0;

        s = edje_object_data_get(page->edje_list, "cover_size");
        if (s) size = atoi(s);
        if (size < 32) size = 32;

        cover = cover_album_fetch(page->layout, db, album, size, NULL, NULL);
        elm_layout_content_set(page->layout_list, "ejy.swallow.cover", cover);

        part = (Evas_Object *)
          edje_object_part_object_get(page->edje_list, "ejy.swallow.cover");
        evas_object_event_callback_add
          (part, EVAS_CALLBACK_RESIZE, _song_album_cover_size_changed, page);
     }

   if (album->name)
     edje_object_part_text_set(page->edje_list, "ejy.text.album", album->name);
   else
     edje_object_part_text_set(page->edje_list, "ejy.text.album", "");

   if (album->artist)
     edje_object_part_text_set(page->edje_list, "ejy.text.artist", album->artist);
   else
     edje_object_part_text_set(page->edje_list, "ejy.text.artist", "");

   return EINA_TRUE;
}

static Evas_Object *
_page_album_songs_add(Evas_Object *parent, Album *album)
{
   DB *db = _page_db_get(parent);
   Eina_Iterator *it;
   static const Elm_Genlist_Item_Class song_item_cls = {
     "media-album",
     {
       _song_item_label_get,
       NULL,
       _song_item_state_get,
       _song_item_del
     }
   };
   static const Page_Class song_cls = {
     "song",
     "_enjoy_page_songs",
     "page/songs-album",
     _song_album_init,
     _page_songs_after_populate,
     _song_item_selected,
     &song_item_cls,
     PAGE_SONGS_POPULATE_ITERATION_COUNT,
     (void *(*)(const void*))db_song_copy,
     offsetof(Song, title),
     0
   };

   it = db_album_songs_get(db, album->id);

   if ((!album->artist) && (!album->flags.fetched_artist))
     db_album_artist_fetch(db, album);

   return _page_add(parent, album, it, "Album Songs", &song_cls);
}

Song *
page_songs_selected_get(const Evas_Object *obj)
{
   PAGE_SONGS_GET_OR_RETURN(page, obj, NULL);
   return page->selected ? elm_genlist_item_data_get(page->selected) : NULL;
}

Eina_Bool
page_songs_song_updated(Evas_Object *obj)
{
   PAGE_SONGS_GET_OR_RETURN(page, obj, EINA_FALSE);
   if (page->selected)
     {
        elm_genlist_item_update(page->selected);
        return EINA_TRUE;
     }
   return EINA_FALSE;
}

Eina_Bool
page_songs_next_exists(const Evas_Object *obj)
{
   PAGE_SONGS_GET_OR_RETURN(page, obj, EINA_FALSE);
   Elm_Genlist_Item *it = page->selected;
   if (!it) return EINA_FALSE;
   it = elm_genlist_item_next_get(it);
   return !!it;
}

Song *
page_songs_next_go(Evas_Object *obj)
{
   PAGE_SONGS_GET_OR_RETURN(page, obj, NULL);
   Elm_Genlist_Item *it = page->selected;
   Song *song;
   if (!it) return NULL;
   it = elm_genlist_item_next_get(it);
   if (!it) return NULL;
   song = elm_genlist_item_data_get(it);
   page->selected = it;
   elm_genlist_item_selected_set(it, EINA_TRUE);
   elm_genlist_item_bring_in(it);
   mpris_signal_player_track_change(song);
   return song;
}

int32_t
page_songs_selected_n_get(Evas_Object *obj)
{
   PAGE_SONGS_GET_OR_RETURN(page, obj, 0);
   Elm_Genlist_Item *it;
   int n;
   for (n = 0, it = page->first;
        it && it != page->selected;
        n++, it = elm_genlist_item_next_get(it));
   return (it) ? n : 0;
}

Song *
page_songs_nth_get(Evas_Object *obj, int32_t n)
{
   PAGE_SONGS_GET_OR_RETURN(page, obj, NULL);
   Elm_Genlist_Item *it = page->first;
   while (it && n--) it = elm_genlist_item_next_get(it);
   if (!it) return NULL;
   return elm_genlist_item_data_get(it);
}

Eina_Bool
_page_shuffle_array(Page *page, Eina_Bool next)
{
   size_t i, j;
   Elm_Genlist_Item *it;

   if (!page->first) return EINA_FALSE;

   if (!page->shuffle)
     {
        page->shuffle = eina_array_new(page->num_elements);
        if (!page->shuffle) return EINA_FALSE;
        eina_array_push(page->shuffle, page->first);
        page->shuffle_position = 0;
     }

   if ((page->shuffle_position >= page->num_elements) || (page->shuffle_position == 0))
     {
        it = page->first;
        eina_array_data_set(page->shuffle, 0, it);
        it = elm_genlist_item_next_get(it);
        for (i = 1; it; i++)
          {
             j = rand() % (i + 1);
             eina_array_data_set(page->shuffle, i,
                                 eina_array_data_get(page->shuffle, j));
             eina_array_data_set(page->shuffle, j, it);
             it = elm_genlist_item_next_get(it);
          }
        if ((page->selected) && (next))
          {
             size_t count = page->num_elements;
             for (i = 0; i < count / 2; i++)
                if (eina_array_data_get(page->shuffle, i) == page->selected)
                  {
                     eina_array_data_set(page->shuffle, i,
                                         eina_array_data_get(page->shuffle,
                                                             count - 1));
                     eina_array_data_set(page->shuffle, count - 1,
                                         page->selected);
                     break;
                  }
          }
        page->shuffle_position = 0;
     }
   return EINA_TRUE;
}

Song *
page_songs_shuffle_prev_go(Evas_Object *obj)
{
   Song *song;
   Elm_Genlist_Item *it;

   PAGE_SONGS_GET_OR_RETURN(page, obj, NULL);

   if (page->shuffle_position > 0)
      page->shuffle_position--;
   if ((!page->shuffle) || (page->shuffle_position == 0))
     {
        _page_shuffle_array(page, EINA_FALSE);
        page->shuffle_position = page->num_elements;
     }

   it = eina_array_data_get(page->shuffle, page->shuffle_position - 1);

   song = elm_genlist_item_data_get(it);

   page->selected = it;
   elm_genlist_item_selected_set(it, EINA_TRUE);
   elm_genlist_item_bring_in(it);
   mpris_signal_player_track_change(song);
   return song;
}

void
page_songs_shuffle_reset(Evas_Object *obj)
{
   PAGE_SONGS_GET_OR_RETURN(page, obj);
   page->shuffle_position = 0;
}

Song *
page_songs_shuffle_next_go(Evas_Object *obj)
{
   Song *song;
   Elm_Genlist_Item *it;

   PAGE_SONGS_GET_OR_RETURN(page, obj, NULL);
   _page_shuffle_array(page, EINA_TRUE);

   it = eina_array_data_get(page->shuffle, page->shuffle_position);
   page->shuffle_position++;

   song = elm_genlist_item_data_get(it);

   page->selected = it;
   elm_genlist_item_selected_set(it, EINA_TRUE);
   elm_genlist_item_bring_in(it);
   mpris_signal_player_track_change(song);
   return song;
}

Eina_Bool
page_songs_prev_exists(const Evas_Object *obj)
{
   PAGE_SONGS_GET_OR_RETURN(page, obj, EINA_FALSE);
   Elm_Genlist_Item *it = page->selected;
   if (!it) return EINA_FALSE;
   it = elm_genlist_item_prev_get(it);
   return !!it;
}

Song *
page_songs_prev_go(Evas_Object *obj)
{
   PAGE_SONGS_GET_OR_RETURN(page, obj, NULL);
   Elm_Genlist_Item *it = page->selected;
   Song *song;
   if (!it) return NULL;
   it = elm_genlist_item_prev_get(it);
   if (!it) return NULL;
   song = elm_genlist_item_data_get(it);
   page->selected = it;
   elm_genlist_item_selected_set(it, EINA_TRUE);
   elm_genlist_item_bring_in(it);
   mpris_signal_player_track_change(song);
   return song;
}


/***********************************************************************
 * FOLDERS
 **********************************************************************/

static char *
_album_item_label_get(void *data, Evas_Object *list, const char *part)
{
   Album *album = data;
   if (strcmp(part, "elm.text.artist") == 0)
     {
        if (!album->flags.fetched_artist)
          {
             DB *db = evas_object_data_get(list, "_enjoy_container");
             db_album_artist_fetch(db, album);
          }
        if (album->artist) return strdup(album->artist);
        else return NULL;
     }

   return strdup(album->name);
}

static void
_album_cover_fetch_finished_cb(void *data)
{
   Elm_Genlist_Item *it = data;
   if (it) elm_genlist_item_update(it);
}

static Evas_Object *
_album_item_icon_get(void *data, Evas_Object *list, const char *part __UNUSED__)
{
   Page *page = evas_object_data_get(list, "_enjoy_page");
   Album *album = data;
   Elm_Genlist_Item *it = eina_hash_find(page->od_to_list_item, album);
   return cover_album_fetch(list, page->container, album, page->cls->icon_size,
                            _album_cover_fetch_finished_cb, it);
}

static void
_album_item_del(void *data, Evas_Object *list __UNUSED__)
{
   db_album_free(data);
}

static void
_album_item_selected(void *data, Evas_Object *list __UNUSED__, void *event_info)
{
   Page *page = data;
   Album *album = elm_genlist_item_data_get(event_info);
   EINA_SAFETY_ON_NULL_RETURN(album);
   Evas_Object *next = _page_album_songs_add(page->layout, album);
   if (next)
     evas_object_smart_callback_call(page->layout, "folder-songs", next);
   elm_genlist_item_selected_set(event_info, EINA_FALSE);
   page->selected = NULL;
}

static Evas_Object *
_page_albums_artist_add(Evas_Object *parent, NameID *nameid, Eina_Iterator *it, const char *title)
{
   static const Elm_Genlist_Item_Class album_item_cls = {
     "media-preview",
     {
       _album_item_label_get,
       _album_item_icon_get,
       NULL,
       _album_item_del
     }
   };
   static Page_Class album_cls = {
     "album",
     "_enjoy_page_albums",
     "page/albums",
     NULL,
     NULL,
     _album_item_selected,
     &album_item_cls,
     PAGE_FOLDERS_POPULATE_ITERATION_COUNT,
     (void *(*)(const void*))db_album_copy,
     offsetof(Album, name),
     0
   };

   if (!album_cls.icon_size)
     {
        Evas_Object *l = elm_layout_add(parent);
        if (elm_layout_theme_set(l, "genlist", "item_compress/media-preview", "default"))
          {
             const char *s = edje_object_data_get(elm_layout_edje_get(l), "icon_size");
             if (s) album_cls.icon_size = atoi(s);
          }
        evas_object_del(l);
        if (!album_cls.icon_size)
          {
             ERR("Could not get icon_size! assume 32");
             album_cls.icon_size = 32;
          }
     }

   return _page_add(parent, nameid, it, title, &album_cls);
}

static Evas_Object *
_page_albums_add(Evas_Object *parent, NameID *nameid, Eina_Iterator *it, const char *title)
{
   static const Elm_Genlist_Item_Class album_item_cls = {
     "media-preview",
     {
       _album_item_label_get,
       _album_item_icon_get,
       NULL,
       _album_item_del
     }
   };
   static Page_Class album_cls = {
     "album",
     "_enjoy_page_albums",
     "page/albums",
     NULL,
     NULL,
     _album_item_selected,
     &album_item_cls,
     PAGE_FOLDERS_POPULATE_ITERATION_COUNT,
     (void *(*)(const void*))db_album_copy,
     offsetof(Album, name),
     0
   };

   if (!album_cls.icon_size)
     {
        Evas_Object *l = elm_layout_add(parent);
        if (elm_layout_theme_set(l, "genlist", "item_compress/media-preview", "default"))
          {
             const char *s = edje_object_data_get(elm_layout_edje_get(l), "icon_size");
             if (s) album_cls.icon_size = atoi(s);
          }
        evas_object_del(l);
        if (!album_cls.icon_size)
          {
             ERR("Could not get icon_size! assume 32");
             album_cls.icon_size = 32;
          }
     }

   return _page_add(parent, nameid, it, title, &album_cls);
}

static char *
_item_all_songs_label_get(void *data __UNUSED__, Evas_Object *list __UNUSED__, const char *part __UNUSED__)
{
   if (strcmp(part, "elm.text.artist") == 0)
        return NULL;
   return strdup("All Songs");
}

static Evas_Object *
_item_all_songs_icon_get(void *data __UNUSED__, Evas_Object *list __UNUSED__, const char *part __UNUSED__)
{
   Page *page = evas_object_data_get(list, "_enjoy_page");
   return cover_allsongs_fetch(list, page->cls->icon_size);
}


static const Elm_Genlist_Item_Class _item_all_songs_cls = {
  "media-preview",
  {
    _item_all_songs_label_get,
    _item_all_songs_icon_get,
    NULL,
    NULL
  }
};

static char *
_nameid_item_label_get(void *data, Evas_Object *list __UNUSED__, const char *part __UNUSED__)
{
   NameID *nameid = data;
   return strdup(nameid->name);
}

static void
_nameid_item_del(void *data, Evas_Object *list __UNUSED__)
{
   db_nameid_free(data);
}

static void
_artist_item_all_songs_selected(void *data, Evas_Object *list __UNUSED__, void *event_info)
{
   Page *page = data;
   NameID *nameid = page->item.data;
   DB *db = _page_db_get(page->layout);
   Eina_Iterator *it = db_artist_songs_get(db, nameid->id);
   char buf[128];
   snprintf(buf, sizeof(buf), "Songs by %s", nameid->name);
   Evas_Object *next = _page_songs_add(page->layout, nameid, it, buf);
   if (next)
     evas_object_smart_callback_call(page->layout, "folder-songs", next);
   elm_genlist_item_selected_set(event_info, EINA_FALSE);
   page->selected = NULL;
}

static void
_artist_item_selected(void *data, Evas_Object *list __UNUSED__, void *event_info)
{
   Page *page = data;
   NameID *nameid = elm_genlist_item_data_get(event_info);
   EINA_SAFETY_ON_NULL_RETURN(nameid);
   DB *db = _page_db_get(page->layout);
   Eina_Iterator *it = db_artist_albums_get(db, nameid->id);
   char buf[128];
   snprintf(buf, sizeof(buf), "Albums by %s", nameid->name);
   Evas_Object *next = _page_albums_add(page->layout, nameid, it, buf);
   if (next)
     {
        Page *next_page = evas_object_data_get(next, "_enjoy_page");
        next_page->item.data = db_nameid_copy(nameid);
        next_page->item.free = (void (*)(void *))db_nameid_free;
        elm_genlist_item_append
          (next_page->list, &_item_all_songs_cls, NULL, NULL,
           ELM_GENLIST_ITEM_NONE, _artist_item_all_songs_selected, next_page);

        evas_object_smart_callback_call(page->layout, "folder", next);
     }
   elm_genlist_item_selected_set(event_info, EINA_FALSE);
   page->selected = NULL;
}

static Evas_Object *
_page_artists_add(Evas_Object *parent, NameID *nameid, Eina_Iterator *it, const char *title)
{
   static const Elm_Genlist_Item_Class nameid_item_cls = {
     "default",
     {
       _nameid_item_label_get,
       NULL,
       NULL,
       _nameid_item_del
     }
   };
   static const Page_Class nameid_cls = {
     "artist",
     "_enjoy_page_artists",
     "page/nameids",
     NULL,
     NULL,
     _artist_item_selected,
     &nameid_item_cls,
     PAGE_FOLDERS_POPULATE_ITERATION_COUNT,
     (void *(*)(const void*))db_nameid_copy,
     offsetof(NameID, name),
     0
   };
   return _page_add(parent, nameid, it, title, &nameid_cls);
}

static void
_genre_item_all_songs_selected(void *data, Evas_Object *list __UNUSED__, void *event_info)
{
   Page *page = data;
   NameID *nameid = page->item.data;
   DB *db = _page_db_get(page->layout);
   Eina_Iterator *it = db_genre_songs_get(db, nameid->id);
   char buf[128];
   snprintf(buf, sizeof(buf), "Songs of %s", nameid->name);
   Evas_Object *next = _page_songs_add(page->layout, nameid, it, buf);
   if (next)
     evas_object_smart_callback_call(page->layout, "folder-songs", next);
   elm_genlist_item_selected_set(event_info, EINA_FALSE);
   page->selected = NULL;
}

static void
_genre_item_selected(void *data, Evas_Object *list __UNUSED__, void *event_info)
{
   Page *page = data;
   NameID *nameid = elm_genlist_item_data_get(event_info);
   EINA_SAFETY_ON_NULL_RETURN(nameid);
   DB *db = _page_db_get(page->layout);
   Eina_Iterator *it = db_genre_albums_get(db, nameid->id);
   char buf[128];
   snprintf(buf, sizeof(buf), "Albums of %s", nameid->name);
   Evas_Object *next = _page_albums_artist_add(page->layout, nameid, it, buf);
   if (next)
     {
        Page *next_page = evas_object_data_get(next, "_enjoy_page");
        next_page->item.data = db_nameid_copy(nameid);
        next_page->item.free = (void (*)(void *))db_nameid_free;
        elm_genlist_item_append
          (next_page->list, &_item_all_songs_cls, NULL, NULL,
           ELM_GENLIST_ITEM_NONE, _genre_item_all_songs_selected, next_page);

        evas_object_smart_callback_call(page->layout, "folder", next);
     }
   elm_genlist_item_selected_set(event_info, EINA_FALSE);
   page->selected = NULL;
}

static Evas_Object *
_page_genres_add(Evas_Object *parent, Eina_Iterator *it, const char *title)
{
   static const Elm_Genlist_Item_Class nameid_item_cls = {
     "default",
     {
       _nameid_item_label_get,
       NULL,
       NULL,
       _nameid_item_del
     }
   };
   static const Page_Class nameid_cls = {
     "genre",
     "_enjoy_page_genres",
     "page/nameids",
     NULL,
     NULL,
     _genre_item_selected,
     &nameid_item_cls,
     PAGE_FOLDERS_POPULATE_ITERATION_COUNT,
     (void *(*)(const void*))db_nameid_copy,
     offsetof(NameID, name),
     0
   };
   return _page_add(parent, NULL, it, title, &nameid_cls);
}

typedef struct _Static_Item
{
   const char *label;
   Evas_Object *(*action)(Evas_Object *parent, void *data);
   const void *data;
   const char *signal;
} Static_Item;

static char *
_static_item_label_get(void *data, Evas_Object *list __UNUSED__, const char *part __UNUSED__)
{
   Static_Item *root = data;
   return strdup(root->label);
}

static void
_static_item_selected(void *data, Evas_Object *list __UNUSED__, void *event_info)
{
   Page *page = data;
   const Static_Item *si = elm_genlist_item_data_get(event_info);
   Evas_Object *next;
   EINA_SAFETY_ON_NULL_RETURN(si);
   EINA_SAFETY_ON_NULL_RETURN(si->action);
   next = si->action(page->layout, (void *)si->data);
   if (next)
     evas_object_smart_callback_call(page->layout, si->signal, next);
   elm_genlist_item_selected_set(event_info, EINA_FALSE);
   page->selected = NULL;
}

static Evas_Object *
_page_all_songs_add(Evas_Object *parent, void *data)
{
   PAGE_GET_OR_RETURN(page, parent, NULL);
   DB *db = _page_db_get(parent);
   Eina_Iterator *it = db_songs_get(db);
   return _page_songs_add(parent, NULL, it, data);
}

static Evas_Object *
_page_all_albums_add(Evas_Object *parent, void *data)
{
   PAGE_GET_OR_RETURN(page, parent, NULL);
   DB *db = _page_db_get(parent);
   Eina_Iterator *it = db_albums_get(db);
   return _page_albums_artist_add(parent, NULL, it, data);
}

static Evas_Object *
_page_all_artists_add(Evas_Object *parent, void *data)
{
   PAGE_GET_OR_RETURN(page, parent, NULL);
   DB *db = _page_db_get(parent);
   Eina_Iterator *it = db_artists_get(db);
   return _page_artists_add(parent, NULL, it, data);
}

static Evas_Object *
_page_all_genres_add(Evas_Object *parent, void *data)
{
   PAGE_GET_OR_RETURN(page, parent, NULL);
   DB *db = _page_db_get(parent);
   Eina_Iterator *it = db_genres_get(db);
   return _page_genres_add(parent, it, data);
}

Evas_Object *
page_root_add(Evas_Object *parent)
{
   static const Elm_Genlist_Item_Class root_item_cls = {
     "default", { _static_item_label_get, NULL, NULL, NULL }
   };
   static const Page_Class root_cls = {
     "root",
     "_enjoy_page_roots",
     "page/roots",
     NULL,
     NULL,
     _static_item_selected,
     &root_item_cls,
     PAGE_FOLDERS_POPULATE_ITERATION_COUNT,
     (void *(*)(const void*))_data_from_itr_passthrough,
     offsetof(Static_Item, label),
     0
   };
   static const Static_Item root_items[] = {
     {"All Songs", _page_all_songs_add, "All Songs", "folder-songs"},
     {"Albums", _page_all_albums_add, "All Albums", "folder"},
     {"Artists", _page_all_artists_add, "All Artists", "folder"},
     {"Genres", _page_all_genres_add, "All Genres", "folder"},
   };
   Eina_Iterator *it = _array_iterator_new
     (root_items, sizeof(Static_Item), ARRAY_SIZE(root_items));
   return _page_add(parent, NULL, it, "Enjoy your music!", &root_cls);
}

void
page_back_show(Evas_Object *obj)
{
   PAGE_GET_OR_RETURN(page, obj);
   edje_object_signal_emit(page->edje, "elm,back,show", "elm");
}

void
page_back_hide(Evas_Object *obj)
{
   PAGE_GET_OR_RETURN(page, obj);
   edje_object_signal_emit(page->edje, "elm,back,hide", "elm");
}

void
page_playing_show(Evas_Object *obj)
{
   PAGE_GET_OR_RETURN(page, obj);
   edje_object_signal_emit(page->edje, "elm,next,show", "elm");
}

void
page_playing_hide(Evas_Object *obj)
{
   PAGE_GET_OR_RETURN(page, obj);
   edje_object_signal_emit(page->edje, "elm,next,hide", "elm");
}
