#include "private.h"

#if 0 /* disabled, it will go back at configuration/preferences later */
Eina_Bool
cover_album_local_find(Evas *evas, DB *db, Album *album, void (*cb)(void *data), void *data)
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
#endif

static Evas_Object *
_cover_empty_add(Evas_Object *parent, unsigned short size)
{
   Evas_Object *cover = elm_icon_add(parent);
   elm_icon_smooth_set(cover, size >= 32);
   elm_icon_prescale_set(cover, size);
   return cover;
}

static Evas_Object *
_cover_without_image_set(Evas_Object *cover)
{
   if ((!elm_icon_standard_set(cover, "no-cover")) ||
       (!elm_icon_standard_set(cover, "media-optical")))
     {
        evas_object_del(cover);
        return NULL;
     }
   return cover;
}

static Evas_Object *
_cover_without_image_add(Evas_Object *parent, unsigned short size)
{
   Evas_Object *cover = _cover_empty_add(parent, size);
   return _cover_without_image_set(cover);
}

/* TODO: do this from configure dialog as well
 *
 * it blocks the UI process too heavily, bad for scrolls... need to
 * move to a daemon, or use ethumb_client to do it (with custom
 * prefix, or leave it as a thumbnail)
 */
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

   if (snprintf(file, sizeof(file), "%s/covers/%d/album_%lld.jpg",
                cache_dir, size, (long long int)album->id) < 0)
     {
        ERR("Path name is too big: %s/covers/%d/album_%lld.jpg",
            cache_dir, size, (long long int)album->id);
        return NULL;
     }

   ee = ecore_evas_buffer_new(1, 1);
   if (!ee)
     {
        ERR("Could not create ecore_evas_buffer");
        return NULL;
     }
   e = ecore_evas_get(ee);
   o = ecore_evas_object_image_new(ee);
   if (!o)
     {
        ERR("Could not create sub-ecore evas bridged with an image");
        goto error;
     }

   sub_ee = ecore_evas_object_ecore_evas_get(o);
   sub_e = ecore_evas_get(sub_ee);
   img = evas_object_image_add(sub_e);
   if (!img)
     {
        ERR("Could not create inner evas image object");
        goto error;
     }

   evas_object_image_load_size_set(img, size, size);
   evas_object_image_file_set(img, large_cover->path, NULL);
   if (evas_object_image_load_error_get(img) != EVAS_LOAD_ERROR_NONE)
     {
        ERR("Could not load inner image '%s': %s",
            large_cover->path,
            evas_load_error_str(evas_object_image_load_error_get(img)));
        goto error;
     }

   evas_object_move(img, 0, 0);
   evas_object_resize(img, size, size);
   evas_object_image_fill_set(img, 0, 0, size, size);
   evas_object_show(img);

   evas_object_image_size_set(o, size, size);
   ecore_evas_resize(sub_ee, size, size);

   evas_damage_rectangle_add(sub_e, 0, 0, size, size);
   ecore_evas_manual_render(sub_ee);

   cache_dir = ecore_file_dir_get(file);
   ecore_file_mkpath(cache_dir);
   free(cache_dir);

   if (!evas_object_image_save(o, file, NULL, "quality=90"))
     {
        ERR("Could not save image '%s'", file);
        goto error;
     }

   cover = malloc(sizeof(*cover) + strlen(file) + 1);
   if (!cover)
     goto error;

   cover->w = cover->h = size;
   cover->path_len = strlen(file);
   cover->origin = large_cover->origin;
   memcpy(cover->path, file, cover->path_len + 1);

   INF("Created resized cover album=%lld, size=%d, original=%d: %s",
       (long long int)album->id, size, large_cover->w, cover->path);

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
   if ((!elm_icon_standard_set(cover, "all-songs")) ||
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
   Album_Cover *itr, *best_match, *larger;

   EINA_SAFETY_ON_NULL_RETURN_VAL(album, NULL);

   if (!album->flags.fetched_covers) db_album_covers_fetch(db, album);

   cover = _cover_empty_add(parent, size);

 retry:
   best_match = NULL;
   larger = NULL;
   EINA_INLIST_FOREACH(album->covers, itr)
     {
        unsigned int cur_error;
        unsigned short cur_size = (itr->w > itr->h) ? itr->w : itr->h;

        if (cur_size > size)
          {
             larger = itr;
             cur_error = cur_size - size;
          }
        else cur_error = size - cur_size;

        if (min_error > cur_error)
          {
             min_error = cur_error;
             best_match = itr;
             if (cur_error == 0) break;
          }
     }

   if ((min_error > 0) && (larger))
     {
        Evas_Object *resized = _cover_with_exact_size
          (parent, db, album, larger, size);
        INF("created exact album=%lld cover size=%d from size=%d: %p",
            (long long int)album->id, size, larger->w, resized);
        if (resized)
          {
             evas_object_del(cover);
             return resized;
          }
     }

   if (!best_match) return _cover_without_image_set(cover);

   if (!elm_icon_file_set(cover, best_match->path, NULL))
     {
        album->covers = eina_inlist_remove
          (album->covers, EINA_INLIST_GET(best_match));

        INF("Removed bogus cover '%s'", best_match->path);
        free(best_match);
        best_match = NULL;

        db_album_covers_update(db, album);
        if (album->covers) goto retry;
        else return _cover_without_image_set(cover);
     }

   DBG("best match album=%lld, size=%dx%d (requested=%d, error=%d): %s",
       (long long int)album->id,
       best_match->w, best_match->h, size, min_error, best_match->path);

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
