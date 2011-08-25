#include "private.h"
#include "coverart-lastfm.h"

struct LastFM_Completed_Data {
    int ref_count;
    Album *album;
    Evas *evas;
    DB *db;
    struct {
       void (*cb)(void *);
       void *data;
    } success;
};

static void
_cover_album_lastfm_download_finished_cb(void *data, char *local_path, Eina_Bool success)
{
    struct LastFM_Completed_Data *completed_data = data;
    Evas_Object *img;
    Album_Cover *cover;
    Evas_Load_Error err;
    int w, h;

    if (!success) goto download_failed;
    img = evas_object_image_add(completed_data->evas);
    evas_object_image_file_set(img, local_path, NULL);
    err = evas_object_image_load_error_get(img);
    if (err != EVAS_LOAD_ERROR_NONE)
      {
         ERR("could not open image %s: %s",
             local_path, evas_load_error_str(err));
         goto end;
      }
    evas_object_image_size_get(img, &w, &h);
    if ((w == 0) || (h == 0))
      {
         ERR("could not get image size %s", local_path);
         goto end;
      }
    cover = malloc(sizeof(*cover) + strlen(local_path) + 1);
    if (!cover)
      {
         ERR("could not allocate memory");
         goto end;
      }

    cover->w = w;
    cover->h = h;
    cover->origin = ALBUM_COVER_ORIGIN_LASTFM;
    strcpy(cover->path, local_path);

    completed_data->album->covers = eina_inlist_append
      (completed_data->album->covers, EINA_INLIST_GET(cover));
    db_album_covers_update(completed_data->db, completed_data->album);
    if (completed_data->success.cb)
      completed_data->success.cb(completed_data->success.data);
end:
    if (img) evas_object_del(img);
download_failed:
    if (completed_data->ref_count-- <= 0) free(completed_data);
}

static void
_cover_album_lastfm_request_finished_cb(void *data, Eina_Hash *urls)
{
    struct LastFM_Completed_Data *completed_data = data;
    Eina_Iterator *iter = eina_hash_iterator_tuple_new(urls);
    Eina_Hash_Tuple *tuple;
    if (!iter) return;
    int max_size = -1;
    char *url = NULL;

    EINA_ITERATOR_FOREACH(iter, tuple)
    {
       int size = *((int *)tuple->data);
       if (size > max_size)
         {
           max_size = size;
           url = (char *)tuple->key;
         }
    }

    if (max_size > 0 && url)
      {
         Eina_Bool success;

         completed_data->ref_count++;
         success = lastfm_cover_download(url,
                                         _cover_album_lastfm_download_finished_cb,
                                         completed_data);
         if (!success) ERR("could not download cover from %s", url);
      }

    eina_iterator_free(iter);
    lastfm_response_free(urls);
}

static void
_cover_album_lastfm_find(Evas *evas, DB *db, Album *album, void (*cb)(void *), void *data)
{
   struct LastFM_Completed_Data *completed_data;

   if ((album->len.artist + album->len.name) == 0) return;

   completed_data = calloc(1, sizeof(*completed_data));
   if (!completed_data) return;
   completed_data->album = album;
   completed_data->evas = evas;
   completed_data->db = db;
   completed_data->success.cb = cb;
   completed_data->success.data = data;
   completed_data->ref_count = 1;

   lastfm_cover_search_request(album->artist, album->name,
                               _cover_album_lastfm_request_finished_cb,
                               completed_data);
}

static Eina_Bool
_cover_album_local_find(Evas *evas, DB *db, Album *album, void (*cb)(void *data), void *data)
{
   Evas_Object *img = evas_object_image_add(evas);
   Eina_Iterator *it = db_album_songs_get(db, album->id);
   const Song *song;
   Eina_Bool success;
   Eina_Hash *done_dirs;

   done_dirs = eina_hash_string_superfast_new(NULL);

   EINA_ITERATOR_FOREACH(it, song)
     {
        char dir[PATH_MAX];
        size_t dir_len = 0, i;
        Eina_Iterator *files;
        const Eina_File_Direct_Info *fi;

        memcpy(dir, song->path, song->len.path + 1);
        for (i = song->len.path; i > 0; i--)
          if (dir[i] == '/')
            {
               dir[i] = '\0';
               dir_len = i;
               break;
            }
        if (!dir_len) continue;

        if (eina_hash_find(done_dirs, dir)) continue;
        eina_hash_add(done_dirs, dir, (void *)0xdeadbeef);

        files = eina_file_direct_ls(dir);
        if (!files)
          {
             DBG("could not open directory '%s': %s", dir, strerror(errno));
             continue;
          }
        dir[dir_len] = '/';

        EINA_ITERATOR_FOREACH(files, fi)
          {
             Album_Cover *cover;
             Evas_Load_Error err;
             int w = 0, h = 0;
             const char *ext;

             /* 5 = sizeof("jpeg") + 1 ('\0') */
             if (fi->path_length <= 5) continue;
             ext = strchr(fi->path + fi->path_length - 5, '.');
             if (!ext) continue;
             ext++;
             if (strcasecmp(ext, "jpg") &&
                 strcasecmp(ext, "jpeg") &&
                 strcasecmp(ext, "png"))
               continue;

             evas_object_image_file_set(img, fi->path, NULL);
             err = evas_object_image_load_error_get(img);
             if (err != EVAS_LOAD_ERROR_NONE)
               {
                  ERR("could not open image %s: %s",
                      fi->path, evas_load_error_str(err));
                  continue;
               }
             evas_object_image_size_get(img, &w, &h);
             if ((w == 0) || (h == 0))
               {
                  ERR("could not get image size %s", fi->path);
                  continue;
               }

             cover = malloc(sizeof(Album_Cover) + fi->path_length + 1);
             if (!cover)
               {
                  ERR("could not allocate memory");
                  continue;
               }
             cover->w = w;
             cover->h = h;
             cover->path_len = fi->path_length;
             memcpy(cover->path, fi->path, fi->path_length + 1);
             cover->origin = ALBUM_COVER_ORIGIN_LOCAL;

             album->covers = eina_inlist_append
               (album->covers, EINA_INLIST_GET(cover));
          }

        eina_iterator_free(files);
     }

   eina_iterator_free(it);
   evas_object_del(img);
   eina_hash_free(done_dirs);

   success = !!eina_inlist_count(album->covers);
   if (cb && success) cb(data);
   db_album_covers_update(db, album);
   return success;
}

static void
_cover_album_find(Evas *evas, DB *db, Album *album, void (*cb)(void *), void *data)
{
   if (!_cover_album_local_find(evas, db, album, cb, data))
     _cover_album_lastfm_find(evas, db, album, cb, data);

   album->flags.fetched_covers = !!eina_inlist_count(album->covers);
}

static Evas_Object *
_cover_empty_add(Evas_Object *parent, unsigned short size)
{
   Evas_Object *cover = elm_icon_add(parent);
   elm_icon_smooth_set(cover, size >= 32);
   elm_icon_prescale_set(cover, size);
   return cover;
}

static Evas_Object *
_cover_without_image_add(Evas_Object *parent, unsigned short size)
{
   Evas_Object *cover = _cover_empty_add(parent, size);
   if ((!elm_icon_standard_set(cover, "no-cover")) &&
       (!elm_icon_standard_set(cover, "media-optical")))
     {
        evas_object_del(cover);
        return NULL;
     }
   return cover;
}

static Evas_Object *
_cover_with_exact_size(Evas_Object *parent, DB *db, Album *album, const Album_Cover *large_cover, int size)
{
   Ecore_Evas *ee, *sub_ee;
   Album_Cover *cover;
   Evas *e, *sub_e;
   Evas_Object *o, *img, *icon;
   char file[PATH_MAX];
   char *cache_dir;

   if ((size > large_cover->w) || (size > large_cover->h))
     {
        icon = _cover_empty_add(parent, size);
        elm_icon_file_set(icon, large_cover->path, NULL);
        return icon;
     }

   cache_dir = enjoy_cache_dir_get();
   if (!cache_dir)
     {
        ERR("Could not get cache dir");
        return NULL;
     }

   if (snprintf(file, sizeof(file), "%s/album_%lld_cover_art_%dpx.jpg",
                cache_dir, album->id, size) < 0)
     return NULL;

   ee = ecore_evas_buffer_new(1, 1);
   if (!ee) return NULL;
   e = ecore_evas_get(ee);
   if (!e) goto error;

   evas_image_cache_set(e, 0);
   evas_font_cache_set(e, 0);

   o = ecore_evas_object_image_new(ee);
   if (!o) goto error;
   sub_ee = evas_object_data_get(o, "Ecore_Evas");
   sub_e = ecore_evas_get(sub_ee);

   evas_image_cache_set(sub_e, 0);
   evas_font_cache_set(sub_e, 0);

   img = evas_object_image_add(sub_e);
   if (!img) goto error;

   evas_object_image_load_size_set(img, size, size);
   evas_object_image_file_set(img, large_cover->path, NULL);
   if (evas_object_image_load_error_get(img) != EVAS_LOAD_ERROR_NONE)
     goto error;

   evas_object_move(img, 0, 0);
   evas_object_resize(img, size, size);
   evas_object_image_fill_set(img, 0, 0, size, size);
   evas_object_show(img);

   evas_object_image_size_set(o, size, size);
   ecore_evas_resize(sub_ee, size, size);

   evas_damage_rectangle_add(sub_e, 0, 0, size, size);
   evas_render(sub_e);

   if (!evas_object_image_save(o, file, NULL, "quality=90"))
     goto error;

   cover = malloc(sizeof(*cover) + strlen(file) + 1);
   if (!cover)
     goto error;

   cover->w = cover->h = size;
   cover->path_len = strlen(file);
   cover->origin = large_cover->origin;
   strcpy(cover->path, file);

   album->covers = eina_inlist_append(album->covers, EINA_INLIST_GET(cover));
   db_album_covers_update(db, album);

   icon = _cover_empty_add(parent, size);
   elm_icon_file_set(icon, file, NULL);

   ecore_evas_free(ee);
   return icon;
error:
   ecore_evas_free(ee);
   return NULL;
}

Evas_Object *
cover_allsongs_fetch(Evas_Object *parent, unsigned short size)
{
   Evas_Object *cover = _cover_empty_add(parent, size);
   if ((!elm_icon_standard_set(cover, "all-songs")) &&
       (!elm_icon_standard_set(cover, "media-optical")))
     {
        evas_object_del(cover);
        return NULL;
     }
   return cover;
}

Evas_Object *
cover_album_fetch(Evas_Object *parent, DB *db, Album *album, unsigned short size,
                  void (*cb_success)(void *data), void *data)
{
   Evas_Object *cover;
   unsigned int min_error = (unsigned int)-1;
   const Album_Cover *itr, *best_match;
   unsigned int fetches = 0;

   EINA_SAFETY_ON_NULL_RETURN_VAL(album, NULL);

   if (!album->flags.fetched_covers) db_album_covers_fetch(db, album);

 fetch_local:
   fetches++;
   best_match = NULL;
   if (!album->covers)
     _cover_album_find(evas_object_evas_get(parent), db, album, cb_success, data);

   EINA_INLIST_FOREACH(album->covers, itr)
     {
        unsigned int cur_error;
        unsigned short cur_size = (itr->w > itr->h) ? itr->w : itr->h;

        if (cur_size > size) cur_error = cur_size - size;
        else cur_error = size - cur_size;

        if (min_error > cur_error)
          {
             min_error = cur_error;
             best_match = itr;
             if (cur_error == 0) break;
          }
     }

   if (!best_match) return _cover_without_image_add(parent, size);
   if (min_error > 0)
     {
        const Album_Cover *larger_size = NULL;
        unsigned short max_size = 0;
        EINA_INLIST_FOREACH(album->covers, itr)
          {
             unsigned short cur_size = (itr->w > itr->h) ? itr->w : itr->h;
             if (cur_size > max_size)
               {
                  max_size = cur_size;
                  larger_size = itr;
               }
          }
        if (larger_size)
          {
             Evas_Object *resized;
             resized = _cover_with_exact_size(parent, db, album,
                                              larger_size, size);
             if (resized) return resized;
          }
     }

   cover = _cover_empty_add(parent, size);
   if (!elm_icon_file_set(cover, best_match->path, NULL))
     {
        while (album->covers)
          {
             void *n = album->covers;
             album->covers = eina_inlist_remove(album->covers, album->covers);
             free(n);
          }
        db_album_covers_update(db, album);
        evas_object_del(cover);
        if (fetches < 2) goto fetch_local;
        else return _cover_without_image_add(parent, size);
     }

   return cover;
}

Evas_Object *
cover_album_fetch_by_id(Evas_Object *parent, DB *db, int64_t album_id, unsigned short size, void (*cb_success)(void *data), void *data)
{
   Evas_Object *cover;
   Album *album = calloc(1, sizeof(Album));
   album->id = album_id;
   cover = cover_album_fetch(parent, db, album, size, cb_success, data);
   db_album_free(album);
   return cover;
}

void
cover_init(void)
{
   lastfm_cover_init();
}

void
cover_shutdown(void)
{
   lastfm_cover_shutdown();
}
