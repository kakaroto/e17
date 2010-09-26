#include "private.h"

#include <ctype.h>

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

typedef struct _Page_Class
{
   const char *name;
   const char *key;
   const char *layout;
   Evas_Smart_Cb selected;
   const Elm_Genlist_Item_Class *item_cls;
   size_t populate_iteration_count;
   void *(*data_from_itr)(const void *data);
   size_t data_letter_offset;
   unsigned short icon_size;
} Page_Class;

typedef struct _Page
{
   const Page_Class *cls;
   Evas_Object *layout;
   Evas_Object *edje;
   Evas_Object *list;
   Evas_Object *index;
   Evas_Object *parent;
   const char *title;
   void *container;
   Elm_Genlist_Item *selected;
   Elm_Genlist_Item *first;
   Eina_Iterator *iterator;
   Ecore_Idler *populate;
   char last_index_letter[2];
   struct {
      void *data;
      void (*free)(void *data);
   } item;
} Page;

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
     }

   return EINA_TRUE;

 end:
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
   free(page);
}

static void
_page_back(void *data, Evas_Object *o __UNUSED__, const char *emission __UNUSED__, const char *source __UNUSED__)
{
   Page *page = data;
   evas_object_smart_callback_call(page->layout, "back", NULL);
}

static void
_page_songs(void *data, Evas_Object *o __UNUSED__, const char *emission __UNUSED__, const char *source __UNUSED__)
{
   Page *page = data;
   evas_object_smart_callback_call(page->layout, "songs", NULL);
}

static Evas_Object *
_page_add(Evas_Object *parent, Eina_Iterator *it, const char *title, Evas_Object *cover, const Page_Class *cls)
{
   Evas_Object *obj;
   Page *page;
   const char *s;

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

   obj = elm_layout_add(parent);
   if (!obj)
     {
        eina_iterator_free(it);
        return NULL;
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
   evas_object_event_callback_add(obj, EVAS_CALLBACK_DEL, _page_del, page);
   page->layout = obj;
   page->iterator = it;
   page->cls = cls;
   page->parent = parent;

   if (!elm_layout_file_set(obj, PACKAGE_DATA_DIR "/default.edj", cls->layout))
     {
        CRITICAL("no theme for '%s' at %s",
                 cls->layout, PACKAGE_DATA_DIR "/default.edj");
        goto error;
     }

   page->title = eina_stringshare_add(title);
   page->edje = elm_layout_edje_get(obj);
   edje_object_part_text_set(page->edje, "ejy.text.title", page->title);
   edje_object_signal_callback_add
     (page->edje, "ejy,back,clicked", "ejy", _page_back, page);
   edje_object_signal_callback_add
     (page->edje, "ejy,songs,clicked", "ejy", _page_songs, page);

   page->list = elm_genlist_add(obj);
   elm_genlist_bounce_set(page->list, EINA_FALSE, EINA_TRUE);
   elm_genlist_horizontal_mode_set(page->list, ELM_LIST_COMPRESS);
   elm_genlist_compress_mode_set(page->list, EINA_TRUE);
   elm_genlist_block_count_set(page->list, 256);

   s = edje_object_data_get(page->edje, "homogeneous");
   elm_genlist_homogeneous_set(page->list, s ? !!atoi(s) : EINA_FALSE);

   elm_layout_content_set(obj, "ejy.swallow.list", page->list);

   if (edje_object_part_exists(page->edje, "ejy.swallow.index"))
     {
        page->index = elm_index_add(obj);
        evas_object_smart_callback_add
          (page->index, "delay,changed", _page_index_changed, page);
        elm_layout_content_set(obj, "ejy.swallow.index", page->index);
     }

   if (edje_object_part_exists(page->edje, "ejy.swallow.cover"))
     elm_layout_content_set(obj, "ejy.swallow.cover", cover);
   else
     evas_object_del(cover);

   page->container = eina_iterator_container_get(it);
   evas_object_data_set(page->list, "_enjoy_container", page->container);
   evas_object_data_set(page->list, "_enjoy_page", page);
   evas_object_smart_callback_add(page->list, "selected", _page_selected, page);

   page->populate = ecore_idler_add(_page_populate, page);

   return obj;

 error:
   evas_object_del(obj); /* should delete everything */
   return NULL;
}

void
page_songs_exists_changed(Evas_Object *obj, Eina_Bool exists)
{
   PAGE_GET_OR_RETURN(page, obj);
   if (exists)
     edje_object_signal_emit(page->edje, "ejy,songs,show", "ejy");
   else
     edje_object_signal_emit(page->edje, "ejy,songs,hide", "ejy");

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
   if (strncmp(part, "ejy.text.", sizeof("ejy.text.") - 1) != 0)
     return NULL;
   part += sizeof("ejy.text.") - 1;

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

static Evas_Object *
_page_songs_add(Evas_Object *parent, Eina_Iterator *it, const char *title)
{
   static const Elm_Genlist_Item_Class song_item_cls = {
     "song",
     {
       _song_item_label_get,
       NULL,
       NULL,
       _song_item_del
     }
   };
   static const Page_Class song_cls = {
     "song",
     "_enjoy_page_songs",
     "page/songs",
     _song_item_selected,
     &song_item_cls,
     PAGE_SONGS_POPULATE_ITERATION_COUNT,
     (void *(*)(const void*))db_song_copy,
     offsetof(Song, title),
     0
   };
   return _page_add(parent, it, title, NULL, &song_cls);
}

static Evas_Object *
_page_album_songs_add(Evas_Object *parent, Eina_Iterator *it, Evas_Object *cover, const char *title)
{
   static const Elm_Genlist_Item_Class song_item_cls = {
     "song-album",
     {
       _song_item_label_get,
       NULL,
       NULL,
       _song_item_del
     }
   };
   static const Page_Class song_cls = {
     "song",
     "_enjoy_page_songs",
     "page/songs-album",
     _song_item_selected,
     &song_item_cls,
     PAGE_SONGS_POPULATE_ITERATION_COUNT,
     (void *(*)(const void*))db_song_copy,
     offsetof(Song, title),
     0
   };
   return _page_add(parent, it, title, cover, &song_cls);
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
   song = elm_genlist_item_data_get(it);
   page->selected = it;
   elm_genlist_item_selected_set(it, EINA_TRUE);
   elm_genlist_item_bring_in(it);
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
   song = elm_genlist_item_data_get(it);
   page->selected = it;
   elm_genlist_item_selected_set(it, EINA_TRUE);
   elm_genlist_item_bring_in(it);
   return song;
}


/***********************************************************************
 * FOLDERS
 **********************************************************************/

static char *
_album_item_label_get(void *data, Evas_Object *list, const char *part)
{
   Album *album = data;
   if (strcmp(part, "ejy.text.artist") == 0)
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

static Evas_Object *
_album_item_icon_get(void *data, Evas_Object *list, const char *part __UNUSED__)
{
   Page *page = evas_object_data_get(list, "_enjoy_page");
   Album *album = data;
   return cover_album_fetch(list, page->container, album, page->cls->icon_size);
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
   DB *db = _page_db_get(page->layout);
   Eina_Iterator *it = db_album_songs_get(db, album->id);
   Evas_Object *cover;
   char buf[128];

   if ((!album->artist) && (!album->flags.fetched_artist))
     db_album_artist_fetch(db, album);

   if (album->artist)
     snprintf(buf, sizeof(buf), "Songs of %s by %s",
              album->name, album->artist);
   else
     snprintf(buf, sizeof(buf), "Songs of %s", album->name);

   cover = cover_album_fetch(page->layout, db, album, 256); // TODO: size!
   Evas_Object *next = _page_album_songs_add(page->layout, it, cover, buf);
   if (next)
     evas_object_smart_callback_call(page->layout, "folder-songs", next);
   else
     evas_object_del(cover);
   elm_genlist_item_selected_set(event_info, EINA_FALSE);
   page->selected = NULL;
}

static Evas_Object *
_page_albums_artist_add(Evas_Object *parent, Eina_Iterator *it, const char *title)
{
   static const Elm_Genlist_Item_Class album_item_cls = {
     "album-artist",
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
     _album_item_selected,
     &album_item_cls,
     PAGE_FOLDERS_POPULATE_ITERATION_COUNT,
     (void *(*)(const void*))db_album_copy,
     offsetof(Album, name),
     0
   };

   if (!album_cls.icon_size)
     {
        Evas_Object *ed = edje_object_add(evas_object_evas_get(parent));
        if (edje_object_file_set
            (ed, PACKAGE_DATA_DIR"/default.edj",
             "elm/genlist/item_compress/album-artist/default"))
          {
             const char *s = edje_object_data_get(ed, "icon_size");
             if (s) album_cls.icon_size = atoi(s);
          }
        evas_object_del(ed);
        if (!album_cls.icon_size)
          {
             ERR("Could not get icon_size! assume 32");
             album_cls.icon_size = 32;
          }
     }

   return _page_add(parent, it, title, NULL,  &album_cls);
}

static Evas_Object *
_page_albums_add(Evas_Object *parent, Eina_Iterator *it, const char *title)
{
   static const Elm_Genlist_Item_Class album_item_cls = {
     "album",
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
     _album_item_selected,
     &album_item_cls,
     PAGE_FOLDERS_POPULATE_ITERATION_COUNT,
     (void *(*)(const void*))db_album_copy,
     offsetof(Album, name),
     0
   };

   if (!album_cls.icon_size)
     {
        Evas_Object *ed = edje_object_add(evas_object_evas_get(parent));
        if (edje_object_file_set
            (ed, PACKAGE_DATA_DIR"/default.edj",
             "elm/genlist/item_compress/album/default"))
          {
             const char *s = edje_object_data_get(ed, "icon_size");
             if (s) album_cls.icon_size = atoi(s);
          }
        evas_object_del(ed);
        if (!album_cls.icon_size)
          {
             ERR("Could not get icon_size! assume 32");
             album_cls.icon_size = 32;
          }
     }

   return _page_add(parent, it, title, NULL, &album_cls);
}

static char *
_item_all_songs_label_get(void *data __UNUSED__, Evas_Object *list __UNUSED__, const char *part __UNUSED__)
{
   return strdup("All Songs");
}

static const Elm_Genlist_Item_Class _item_all_songs_cls = {
  "all-song",
  {
    _item_all_songs_label_get,
    NULL,
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
   Evas_Object *next = _page_songs_add(page->layout, it, buf);
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
   Evas_Object *next = _page_albums_add(page->layout, it, buf);
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
_page_artists_add(Evas_Object *parent, Eina_Iterator *it, const char *title)
{
   static const Elm_Genlist_Item_Class nameid_item_cls = {
     "nameid",
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
     _artist_item_selected,
     &nameid_item_cls,
     PAGE_FOLDERS_POPULATE_ITERATION_COUNT,
     (void *(*)(const void*))db_nameid_copy,
     offsetof(NameID, name),
     0
   };
   return _page_add(parent, it, title, NULL, &nameid_cls);
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
   Evas_Object *next = _page_songs_add(page->layout, it, buf);
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
   Evas_Object *next = _page_albums_artist_add(page->layout, it, buf);
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
     "nameid",
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
     _genre_item_selected,
     &nameid_item_cls,
     PAGE_FOLDERS_POPULATE_ITERATION_COUNT,
     (void *(*)(const void*))db_nameid_copy,
     offsetof(NameID, name),
     0
   };
   return _page_add(parent, it, title, NULL, &nameid_cls);
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
   return _page_songs_add(parent, it, data);
}

static Evas_Object *
_page_all_albums_add(Evas_Object *parent, void *data)
{
   PAGE_GET_OR_RETURN(page, parent, NULL);
   DB *db = _page_db_get(parent);
   Eina_Iterator *it = db_albums_get(db);
   return _page_albums_artist_add(parent, it, data);
}

static Evas_Object *
_page_all_artists_add(Evas_Object *parent, void *data)
{
   PAGE_GET_OR_RETURN(page, parent, NULL);
   DB *db = _page_db_get(parent);
   Eina_Iterator *it = db_artists_get(db);
   return _page_artists_add(parent, it, data);
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
     "root", { _static_item_label_get, NULL, NULL, NULL }
   };
   static const Page_Class root_cls = {
     "root",
     "_enjoy_page_roots",
     "page/roots",
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
   return _page_add(parent, it, "Enjoy your music!", NULL, &root_cls);
}
