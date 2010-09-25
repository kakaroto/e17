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
        DIR *d;
        struct dirent *de;
        char dir[PATH_MAX];
        size_t dir_len = 0, i;
        const Eina_List *l;
        const char *s;

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

        d = opendir(dir);
        if (!d)
          {
             DBG("could not open directory '%s': %s", dir, strerror(errno));
             continue;
          }
        dir[dir_len] = '/';
        dir_len++;

        while ((de = readdir(d)) != NULL)
          {
             Album_Cover *cover;
             Evas_Load_Error err;
             size_t path_len;
             int w = 0, h = 0;

             if (de->d_name[0] == '.') continue;
             i = eina_strlcpy(dir + dir_len, de->d_name, PATH_MAX - dir_len);
             path_len = dir_len + i;

             for (; i > 0; i--)
               if (dir[dir_len + i] == '.')
                 {
                    const char *ext = dir + dir_len + i + 1;
                    if ((strcasecmp(ext, "jpg") != 0) &&
                        (strcasecmp(ext, "jpeg") != 0) &&
                        (strcasecmp(ext, "png") != 0))
                      i = 0;
                    break;
                 }

             if (i == 0) continue;

             evas_object_image_file_set(img, dir, NULL);
             err = evas_object_image_load_error_get(img);
             if (err != EVAS_LOAD_ERROR_NONE)
               {
                  ERR("could not open image %s: %s",
                      dir, evas_load_error_str(err));
                  continue;
               }
             evas_object_image_size_get(img, &w, &h);
             if ((w == 0) || (h == 0))
               {
                  ERR("could not get image size %s", dir);
                  continue;
               }

             cover = malloc(sizeof(Album_Cover) + path_len + 1);
             if (!cover)
               {
                  ERR("could not allocate memory");
                  continue;
               }
             cover->w = w;
             cover->h = h;
             cover->path_len = path_len;
             memcpy(cover->path, dir, path_len + 1);

             album->covers = eina_inlist_append
               (album->covers, EINA_INLIST_GET(cover));
          }

        closedir(d);
     }

   eina_iterator_free(it);
   evas_object_del(img);

   if (done_dirs)
     {
        char *dir;
        EINA_LIST_FREE(done_dirs, dir) free(dir);
     }
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

   if (!best_match) return NULL;

   cover = elm_icon_add(parent);
   elm_icon_smooth_set(cover, size >= 32);
   elm_icon_prescale_set(cover, size);
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
        else return NULL;
     }

   return cover;
}
