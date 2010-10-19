#include "private.h"

static void
_cover_album_local_find(Evas *evas, DB *db, Album *album)
{
   Evas_Object *img = evas_object_image_add(evas);
   Eina_Iterator *it = db_album_songs_get(db, album->id);
   const Song *song;
   Eina_List *done_dirs = NULL;

   EINA_ITERATOR_FOREACH(it, song)
     {
        char dir[PATH_MAX];
        size_t dir_len = 0, i;
        Eina_Iterator *files;
        const Eina_List *l;
        const char *s;
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

        EINA_LIST_FOREACH(done_dirs, l, s)
          if (strcmp(s, dir) == 0)
            {
               dir_len = 0;
               break;
            }
        if (!dir_len) continue;
        done_dirs = eina_list_append(done_dirs, strdup(dir));

        files = eina_file_direct_ls(dir);
        if (!files)
          {
             DBG("could not open directory '%s': %s", dir, strerror(errno));
             continue;
          }
        dir[dir_len] = '/';
        dir_len++;

        EINA_ITERATOR_FOREACH(files, fi)
          {
             Album_Cover *cover;
             Evas_Load_Error err;
             int w = 0, h = 0;
             const char *ext, *file_type;

             for (ext = fi->path + fi->path_length - 1; ext > fi->path; ext--)
               if (*ext == '.')
                 {
                    ext++;
                    if ((strcasecmp(ext, "jpg") == 0) ||
                        (strcasecmp(ext, "jpeg") == 0) ||
                        (strcasecmp(ext, "png") == 0))
                      goto try_image;
                    else
                      break;
                 }

             file_type = efreet_mime_type_get(fi->path);
             if ((!file_type) || (strncmp("image/", file_type, 5)))
               continue;

          try_image:
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

             album->covers = eina_inlist_append
               (album->covers, EINA_INLIST_GET(cover));
          }

        eina_iterator_free(files);
     }

   eina_iterator_free(it);
   evas_object_del(img);

   if (done_dirs)
     {
        char *dir;
        EINA_LIST_FREE(done_dirs, dir) free(dir);
     }
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
   if (!elm_icon_file_set(cover, PACKAGE_DATA_DIR "/default.edj", "album-without-cover"))
     {
        evas_object_del(cover);
        return NULL;
     }
   return cover;
}

Evas_Object *
cover_allsongs_fetch(Evas_Object *parent, unsigned short size)
{
   Evas_Object *cover = _cover_empty_add(parent, size);
   if (!elm_icon_file_set(cover, PACKAGE_DATA_DIR "/default.edj", "all-songs-icon"))
     {
        evas_object_del(cover);
        return NULL;
     }
   return cover;
}

Evas_Object *
cover_album_fetch(Evas_Object *parent, DB *db, Album *album, unsigned short size)
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
     _cover_album_local_find(evas_object_evas_get(parent), db, album);

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
cover_album_fetch_by_id(Evas_Object *parent, DB *db, int64_t album_id, unsigned short size)
{
   Evas_Object *cover;
   Album *album = calloc(1, sizeof(Album));
   album->id = album_id;
   cover = cover_album_fetch(parent, db, album, size);
   db_album_free(album);
   return cover;
}
