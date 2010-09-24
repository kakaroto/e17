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

typedef struct _Page
{
   Evas_Object *layout;
   Evas_Object *edje;
   Evas_Object *list;
   Evas_Object *index;
   const char *title;
   Elm_Genlist_Item *selected;
   Elm_Genlist_Item *first;
   Eina_Iterator *iterator;
   Ecore_Idler *populate;
   char last_index_letter[2];
} Page;

#define PAGE_SONGS_GET_OR_RETURN(page, obj, ...)                      \
  Page *page = evas_object_data_get(obj, "_enjoy_page_songs");        \
  do { if (!page) return __VA_ARGS__; } while (0)


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
             DB *db = evas_object_data_get(list, "_enjoy_db");
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
             DB *db = evas_object_data_get(list, "_enjoy_db");
             db_song_album_fetch(db, song);
          }
        return song->album ? strdup(song->album) : NULL;
     }
   else if (!strcmp(part, "artist"))
     {
        if (!song->flags.fetched_artist)
          {
             DB *db = evas_object_data_get(list, "_enjoy_db");
             db_song_artist_fetch(db, song);
          }
        return song->artist ? strdup(song->artist) : NULL;
     }
   else if (!strcmp(part, "genre"))
     {
        if (!song->flags.fetched_genre)
          {
             DB *db = evas_object_data_get(list, "_enjoy_db");
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

static const Elm_Genlist_Item_Class _song_item_class = {
  "song",
  {
    _song_item_label_get,
    NULL,
    NULL,
    _song_item_del
  }
};

static Eina_Bool
_page_songs_populate(void *data)
{
   Page *page = data;
   const Song *orig;
   Song *copy;
   unsigned int count;

   for (count = 0; count < PAGE_SONGS_POPULATE_ITERATION_COUNT; count++)
     {
        Elm_Genlist_Item *it;
        char letter;

        if (!eina_iterator_next(page->iterator, (void **)&orig)) goto end;
        // TODO: evaluate if we should keep a full copy or just store
        // fields of interest such as id, title, artist and album
        copy = db_song_copy(orig);
        if (!copy) goto end;

        it = elm_genlist_item_append
          (page->list, &_song_item_class, copy,
           NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);

        letter = toupper(copy->title[0]);
        if (isalpha(letter) && (page->last_index_letter[0] != letter))
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
_page_songs_del(void *data, Evas *e __UNUSED__, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Page *page = data;
   eina_stringshare_del(page->title);
   if (page->iterator) eina_iterator_free(page->iterator);
   if (page->populate) ecore_idler_del(page->populate);
   free(page);
}

static void
_page_songs_selected(void *data, Evas_Object *o __UNUSED__, void *event_info)
{
   Page *page = data;
   Elm_Genlist_Item *it = event_info;
   Song *song = elm_genlist_item_data_get(it);
   EINA_SAFETY_ON_NULL_RETURN(song);
   if (page->selected == it) return;
   page->selected = it;
   evas_object_smart_callback_call(page->layout, "song", song);
}

static void
_page_songs_index_changed(void *data __UNUSED__, Evas_Object *o __UNUSED__, void *event_info)
{
   Elm_Genlist_Item *it = event_info;
   elm_genlist_item_top_bring_in(it);
}

Evas_Object *
page_songs_add(Evas_Object *parent, Eina_Iterator *it, const char *title)
{
   Evas_Object *obj;
   Page *page;
   const char *s;

   EINA_SAFETY_ON_NULL_RETURN_VAL(it, NULL);

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
   evas_object_data_set(obj, "_enjoy_page_songs", page);
   evas_object_event_callback_add
     (obj, EVAS_CALLBACK_DEL, _page_songs_del, page);
   page->layout = obj;
   page->iterator = it;

   if (!elm_layout_file_set(obj, PACKAGE_DATA_DIR "/default.edj", "page/songs"))
     {
        CRITICAL("no theme for 'page/songs' at %s",
                 PACKAGE_DATA_DIR "/default.edj");
        goto error;
     }

   page->title = eina_stringshare_add(title);
   page->edje = elm_layout_edje_get(obj);
   edje_object_part_text_set(page->edje, "ejy.text.title", page->title);

   page->list = elm_genlist_add(obj);
   elm_genlist_bounce_set(page->list, EINA_FALSE, EINA_TRUE);
   elm_genlist_horizontal_mode_set(page->list, ELM_LIST_COMPRESS);
   elm_genlist_compress_mode_set(page->list, EINA_TRUE);
   elm_genlist_block_count_set(page->list, 256);
   evas_object_data_set(page->list, "_enjoy_db",
                        eina_iterator_container_get(it));

   evas_object_smart_callback_add
     (page->list, "selected", _page_songs_selected, page);

   s = edje_object_data_get(page->edje, "homogeneous");
   elm_genlist_homogeneous_set(page->list, s ? !!atoi(s) : EINA_FALSE);

   elm_layout_content_set(obj, "ejy.swallow.list", page->list);

   page->index = elm_index_add(obj);
   evas_object_smart_callback_add
     (page->index, "delay,changed", _page_songs_index_changed, page);
   elm_layout_content_set(obj, "ejy.swallow.index", page->index);

   page->first = NULL;
   page->populate = ecore_idler_add(_page_songs_populate, page);

   return obj;

 error:
   evas_object_del(obj); /* should delete everything */
   return NULL;
}

Song *
page_songs_selected_get(const Evas_Object *obj)
{
   PAGE_SONGS_GET_OR_RETURN(page, obj, NULL);
   return page->selected ? elm_genlist_item_data_get(page->selected) : NULL;
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
