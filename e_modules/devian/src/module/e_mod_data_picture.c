#include "dEvian.h"

#ifdef HAVE_PICTURE

#define CACHE_SIZE() (DEVIANM->conf->data_picture_cache_size + evas_list_count(DEVIANM->devians) * DEVIANM->conf->data_picture_cache_size)

static int _picture_list_local_add_dir(const char *dir, int recursive);
static Picture *_picture_list_get_picture(Evas_List *pictures_list);

static Picture *_picture_cache_get_picture_unused(void);
static int _picture_cache_add_picture(Picture *picture);
static int _picture_cache_del_picture(Picture *picture);
static int _picture_cache_fill(void);

static int _picture_free(Picture *picture, int force, int force_now);
static void _picture_local_thumb_cb(Evas_Object *obj, void *data);
static Evas_Object *_picture_thumb_get_evas(char *thumb);

/* PUBLIC FUNCTIONS */

/**
 * Initialise the list of local pictures wich we can display
 *
 * This function creates a list of pictures and generate thumbs for all pictures
 * in pictures directories.
 * @return 0 on success
 */
int DEVIANF(data_picture_list_local_init) (void)
{
   Picture_List_Local *list;

   if (!DEVIANM->picture_list_local)
     {
        list = E_NEW(Picture_List_Local, 1);
        list->pictures = NULL;
        list->nb_pictures_waiting = 0;
        DEVIANM->picture_list_local = list;
     }
   else
      list = DEVIANM->picture_list_local;

   /* Load pictures */
   if (DEVIANM->conf->sources_picture_show_devian_pics)
      _picture_list_local_add_dir(e_module_dir_get(DEVIANM->module), 0);
   _picture_list_local_add_dir(DEVIANM->conf->sources_picture_data_import_dir,
                               DEVIANM->conf->sources_picture_data_import_recursive);

   if (list->nb_pictures_waiting > 2)
     {
        char buf[4096];

        if (DEVIANM->conf->sources_picture_data_import_thumbs_warn)
          {
             DEVIANM->conf->sources_picture_data_import_thumbs_warn = 0;
             snprintf(buf, sizeof(buf),
                      _("<hilight>Creating thumbs</hilight><br><br>"
                        "%d pictures are being thumbed in a background task.<br>"
                        "It can take a while, but after, loading will be faster :)<br><br>"
                        "Each time wou will load pictures that haven't been loaded in devian before,<br>"
                        "they will be thumbed"), list->nb_pictures_waiting);
             e_module_dialog_show(_(MODULE_NAME " Module Information"), buf);
             return 0;
          }
        else
          {
             char buf[100];
             int time = 3;

             snprintf(buf, sizeof(buf), "%d pictures in thumbnail", list->nb_pictures_waiting - 2);
             DEVIANF(popup_warn_add) (NULL, POPUP_WARN_TYPE_INFO_TIMER, buf, &time);
          }
     }

   return 0;
}

/**
 * Shutdown the list of local pictures
 *
 * It deletes all pictures and free the list
 */
void DEVIANF(data_picture_list_local_shutdown) (void)
{
   Picture_List_Local *list;
   Evas_List *l;
   Picture *picture;

   list = DEVIANM->picture_list_local;

   if (!list)
      return;

   for (l = list->pictures; l; l = evas_list_next(l))
     {
        picture = evas_list_data(l);
        _picture_free(picture, 1, 1);
     }

   evas_list_free(l);
   E_FREE(list);
   DEVIANM->picture_list_local = NULL;
}

/**
 * This function regenerate the list of local pictures
 */
void DEVIANF(data_picture_list_local_regen) (void)
{
   Picture_List_Local *list;
   Evas_List *l;
   Picture *picture;

   list = DEVIANM->picture_list_local;

   if (!list)
      return;

   if (list->nb_pictures_waiting)
      return;

   for (l = list->pictures; l; l = evas_list_next(l))
     {
        picture = evas_list_data(l);
        _picture_free(picture, 1, 0);
     }

   DEVIANF(data_picture_list_local_init) ();
}

int DEVIANF(data_picture_list_net_init) (void);
void DEVIANF(data_picture_list_net_shutdown) (void);

/**
 * Init picture cache
 */
int DEVIANF(data_picture_cache_init) (void)
{
   Picture_Cache *cache;

   cache = E_NEW(Picture_Cache, 1);
   cache->pictures = NULL;
   cache->pos = -1;
   cache->nb_attached = 0;
   DEVIANM->picture_cache = cache;

   _picture_cache_fill();

   return 1;
}

/** 
 * Shutdown picture cache
 */
void DEVIANF(data_picture_cache_shutdown) (void)
{
   Picture_Cache *cache;
   Picture *picture;
   Evas_List *l;

   cache = DEVIANM->picture_cache;

   if (!cache)
      return;

   for (l = cache->pictures; l; l = evas_list_next(l))
     {
        picture = evas_list_data(l);
        _picture_cache_del_picture(picture);
     }

   evas_list_free(l);
   E_FREE(cache);
   DEVIANM->picture_cache = NULL;
}

/**
 * Attach a picture to a source
 *
 * @param source Source where to attach a picture from cache
 * @param edje_part Part of the source where to attach the picture
 * @param histo_nb If != 0, it will return the picture at source->histo_pos + histo_nb in the histo. If 0, it will return a random picture.
 * @return A pointer to the picture, or NULL if fails (no pictures avalaible ...)
 */
Picture *DEVIANF(data_picture_cache_attach) (Source_Picture *source, int edje_part, int histo_nb)
{
   Picture_Cache *cache;
   Picture *picture;

   cache = DEVIANM->picture_cache;

   /* If old pictures are still attached, abord */
   if (!edje_part && source->picture0)
      return NULL;
   if (edje_part && source->picture1)
      return NULL;

   if (histo_nb || source->histo_pos)
     {
        int new_pos, n;

        /* Get a picture in the histo */

        new_pos = source->histo_pos + histo_nb;
        n = new_pos;
        /* Select the picture if unused and not going to be deleted */
        do
          {
             if (n < 0)
               {
                  /* Can't get a picture in historic ? So get a new one ! */
                  source->histo_pos = 0;
                  return DEVIANF(data_picture_cache_attach) (source, edje_part, 0);
               }
             else
               {
                  if (n > SOURCE_PICTURE_HISTO_MAX_SIZE)
                     return NULL;
               }

             if (!(picture = evas_list_nth(source->histo, n)))
                return NULL;

             new_pos = n;

             if (histo_nb > 0)
                n++;
             else
                n--;
          }
        while (picture->source || picture->delete);

        /* Add it to the cache if wasnt */
	if (!picture->picture)
           if (!_picture_cache_add_picture(picture))
              return NULL;

        /* Update the source's histo position */
        source->histo_pos = new_pos;
     }
   else
     {
        /* Get a picture in the cache */

        /* Need to fill the cache ? */
        if (cache->pos == -1)
           _picture_cache_fill();
        else if (cache->pos > (evas_list_count(cache->pictures) - (int)(evas_list_count(cache->pictures) / 4)))
           _picture_cache_fill();

        /* Get the next picture and change next picture */
        if (cache->pos != -1)
          {
             picture = evas_list_nth(cache->pictures, cache->pos);
             cache->pos++;
             if (cache->pos > (evas_list_count(cache->pictures) - 1))
                cache->pos = -1;        /* Overflow -> no more pictures avalaible ! */
             /* If picture isnt good, retry */
             if (picture->source || picture->delete)
                return DEVIANF(data_picture_cache_attach) (source, edje_part, 0);
          }
        else
          {
             picture = _picture_cache_get_picture_unused();
             if (!picture)
                return NULL;
          }

        /* Attach the picture to the source's historic
	 * and attach the source to the picture histo's list */
        DEVIANF(source_picture_histo_picture_attach) (source, picture);
     }

   /* Attach the picture to the source */
   picture->source = source;
   if (!edje_part)
      source->picture0 = picture;
   else
      source->picture1 = picture;
   cache->nb_attached++;
   
   DDATAC(("attach ok (%s, %p), pos: %d", picture->picture_description->name, picture, cache->pos));

   return picture;
}

/**
 * Detach a picture of source
 *
 * @param source Source where to detach the picture
 * @param part Part of the source to use
 */
void DEVIANF(data_picture_cache_detach) (Source_Picture *source, int part)
{
   Picture_Cache *cache;
   Picture *picture = NULL;

   cache = DEVIANM->picture_cache;

   if (!source)
      return;
   if (!part && (source->picture0))
     {
        picture = source->picture0;
     }
   else
     {
        if (part && (source->picture1))
           picture = source->picture1;
        else
	   {
	      DDATAC(("BAD BAD BAD in cache detach"));
	   }
     }

   if (!picture)
      return;

   /* Detach source from picture */
   picture->source = NULL;

   /* Picture needs to be deleted ? */
   if (picture->delete)
     {
        if (picture->from == DATA_PICTURE_LOCAL)
           _picture_free(picture, 1, 1);
     }

   /* Detach picture from source */
   if (!part && (source->picture0))
      source->picture0 = NULL;
   else
     {
        if (part && (source->picture1))
           source->picture1 = NULL;
     }

   evas_object_hide(picture->picture);

   cache->nb_attached--;

   if (picture->picture_description)
      DDATAC(("detach ok (%s)", picture->picture_description->name));
   else
      DDATAC(("detach ok (-null-)"));
}

/**
 * Get the name of a picture
 *
 * @param path Path of the picture
 * @param len Len of the string to return
 * @return The name
 */
char *DEVIANF(data_picture_get_name_from_path) (char *path, int len)
{
   char name[DEVIAN_MAX_PATH];
   int name_l;
   char *fichier;
   char *ext;
   int ext_l;

   fichier = strrchr(path, '/') + 1;
   if (!fichier)
      return strdup(path);

   ext = strrchr(path, '.');
   if (!ext)
      return strdup(path);
   ext_l = strlen(ext);

   name_l = strlen(fichier) - ext_l;

   strncpy(name, fichier, name_l);
   name[name_l] = '\0';

   return (char *)DEVIANF(ss_utf8_add) (name, 0);
}

/* PRIVATE FUNCTIONS */

static int
_picture_list_local_add_dir(const char *dir, int recursive)
{
   Picture_List_Local *list;
   Picture *picture;
   Ecore_List *files;
   char *file, *ext;
   char *file_tmp;
   int th_w, th_h;
   char buf[DEVIAN_MAX_PATH];
   char buf2[4096];

   list = DEVIANM->picture_list_local;
   th_w = DEVIANM->conf->data_picture_thumb_default_size;
   th_h = DEVIANM->conf->data_picture_thumb_default_size;

   if (!ecore_file_is_dir(dir))
     {
        snprintf(buf2, sizeof(buf2),
                 _("<hilight>Directory %s doesnt exists.</hilight><br><br>"
                   "To import pictures, you have to put them"
                   "in the folder you set in main configuration panel<br><br>"
                   "They can be jpeg, gif, png, edj<br>"
                   "After import, if you can remove these files and the pictures still can<br>"
                   "be viewed, but you wont be able to set them as wallpaper anymore<br><br>"), dir);
        e_module_dialog_show(_(MODULE_NAME " Module Error"), buf2);
        return 0;
     }
   files = ecore_file_ls(dir);
   if (!strcmp(DEVIANM->conf->sources_picture_data_import_dir, dir))
      if (ecore_list_is_empty(files) || !files)
        {
           snprintf(buf2, sizeof(buf2),
                    _("<hilight>Directory %s is empty</hilight><br><br>"
                      "To import pictures, you have to put them"
                      "in this folder.<br>"
                      "They can be jpeg, gif, png, edj<br>"
                      "After import, if you can remove these files and the pictures still can<br>"
                      "be viewed, but you wont be able to set them as wallpaper anymore<br><br>"), dir);
           e_module_dialog_show(_(MODULE_NAME " Module Error"), buf2);
           return 0;
        }

   DDATA(("Going to list %s", dir));

   while ((file = (char *)ecore_list_next(files)) != NULL)
     {
        snprintf(buf, DEVIAN_MAX_PATH, "%s/%s", dir, file);

        if ((file_tmp = ecore_file_readlink(buf)))
          {
             E_FREE(file);
             file = strdup(ecore_file_get_file(file_tmp));
             strncpy(buf, file_tmp, sizeof(buf));
          }

	if (!DEVIANM->conf->sources_picture_data_import_hidden)
	   if (file[0] == '.')
	      continue;

        if (recursive)
           if (ecore_file_is_dir(buf))
             {
                _picture_list_local_add_dir(buf, 1);
                continue;
             }

        ext = strrchr(file, '.');
        if (!ext)
           continue;
        if (strcasecmp(ext, ".edj") &&
            strcasecmp(ext, ".jpg") && strcasecmp(ext, ".JPG") &&
            strcasecmp(ext, ".jpeg") && strcasecmp(ext, ".JPEG") && strcasecmp(ext, ".png") && strcasecmp(ext, ".PNG"))
           continue;

        DDATA(("File %s thumb ...", file));

        picture = E_NEW(Picture, 1);

        picture->source = NULL;
        picture->path = (char *)evas_stringshare_add(buf);
        picture->thumb_path = e_thumb_file_get(picture->path);
        picture->picture = NULL;
        picture->picture_description = E_NEW(Picture_Infos, 1);
        picture->picture_description->name = DEVIANF(data_picture_get_name_from_path) (picture->path, DATA_PICTURE_INFOS_LEN);
        picture->picture_description->author_name = NULL;
        picture->picture_description->where_from = NULL;
        picture->picture_description->date = NULL;
        picture->picture_description->comments = NULL;
        picture->delete = 0;
        picture->from = DATA_PICTURE_LOCAL;
        picture->thumbed = 0;
        picture->cached = 0;
        picture->sources_histo = NULL;

        DDATA(("Thumb %s of %s exists ?", picture->thumb_path, picture->path));
        if (e_thumb_exists(picture->path))
          {
             int w, h;

             e_thumb_geometry_get(picture->thumb_path, &w, &h, 1);
             DDATA(("THUMB %dx%d (wanted %dx%d)", w, h, th_w, th_h));
             if ((th_w > w) && (th_h > h))
               {
                  /* Thumb exists, but regen to new size */
                  int i;

                  i = ecore_file_unlink(picture->thumb_path);
                  DDATA(("File %s thumb exists (%dx%d),  but regen to %dx%d (del old %d)", file, w, h, th_w, th_h, i));
                  e_thumb_generate_begin(picture->path, th_w, th_h,
                                         DEVIANM->container->bg_evas, &picture->picture, _picture_local_thumb_cb, picture);
                  list->nb_pictures_waiting++;
                  continue;
               }
             /* Thumb exists and good size */
             DDATA(("File %s thumb exists and good size, add (%de)", file, evas_list_count(list->pictures)));
             picture->thumbed = 1;
             picture->original_w = w;
             picture->original_h = h;
             list->pictures = evas_list_append(list->pictures, picture);
          }
        else
          {
             /* Thumb doesnt exists so generate it */
             DDATA(("File %s thumb doesnt exist, gen %dx%d", file, th_w, th_h));
             e_thumb_generate_begin(picture->path, th_w, th_h,
                                    DEVIANM->container->bg_evas, &picture->picture, _picture_local_thumb_cb, picture);
             list->nb_pictures_waiting++;
             continue;
          }
     }

   DDATA(("End listing %s", dir));

   return 1;
}

/**
 * Get a picture in a list
 *
 * @param pictures_list list where to get the picture. If NULL, will take the pic in the list indicated by default location
 * @return the picture
 */
static Picture *
_picture_list_get_picture(Evas_List *pictures_list)
{
   Picture *picture;
   Evas_List *l, *was_first;

   picture = NULL;

   if (!pictures_list)
     {
        switch (DEVIANM->conf->sources_picture_default_location)
          {
          case DATA_PICTURE_LOCAL:
             pictures_list = DEVIANM->picture_list_local->pictures;
             break;
          case DATA_PICTURE_NET:
             pictures_list = DEVIANM->picture_list_net->pictures;
             break;
          case DATA_PICTURE_BOTH:
             {
                /* Random between local and net. If one doesnt work, try the other one */
                int i;

                i = rand() % 2;
                if (!i)
                   picture = _picture_list_get_picture(DEVIANM->picture_list_local->pictures);
                else
                   picture = _picture_list_get_picture(DEVIANM->picture_list_net->pictures);

                if (!picture)
                  {
                     if (i)
                        picture = _picture_list_get_picture(DEVIANM->picture_list_local->pictures);
                     else
                        picture = _picture_list_get_picture(DEVIANM->picture_list_net->pictures);
                  }

                return picture;
             }
          }
     }

   if (!evas_list_count(pictures_list))
      return NULL;

   l = evas_list_nth_list(pictures_list, rand() % evas_list_count(pictures_list));
   was_first = l;

   do
     {
        picture = evas_list_data(l);
        if (!picture->source && !picture->cached && !picture->delete)
           return picture;
        l = evas_list_next(l);
        if (!l)
           l = pictures_list;
     }
   while (l != was_first);

   return NULL;
}

/**
 * Get the first unused picture in cache
 *
 * @return Pointer to the picture
 */
static Picture *
_picture_cache_get_picture_unused(void)
{
   Picture *picture;
   Picture_Cache *cache;
   Evas_List *l, *was_first;
   int i = 0;

   cache = DEVIANM->picture_cache;

   DDATAC(("Going to try to attach unused picture"));

   if (!evas_list_count(cache->pictures))
      return NULL;
   if (!cache->pos)
      return NULL;

   if (cache->pos == -1)
     {
        l = evas_list_nth_list(cache->pictures, rand() % evas_list_count(cache->pictures));
     }
   else
     {
        i = rand() % cache->pos;
        l = evas_list_nth_list(cache->pictures, i);
     }

   was_first = l;

   do
     {
        picture = evas_list_data(l);
        /* Current picture ? */
        if (!picture->source && !picture->delete && picture->thumbed)
	   {
	      DDATAC(("Picture found ! %s", picture->picture_description->name));
	      return picture;
	   }
        l = evas_list_next(l);

        if (cache->pos == -1)
          {
             if (!l)
                l = cache->pictures;
          }
        else
          {
             i++;
             if (i == cache->pos)
               {
                  i = 0;
                  l = cache->pictures;
               }
          }

     }
   while (l != was_first);

   return NULL;
}

/**
 * Add a picture to the cache
 *
 * @param picture The picture to cache
 * @return 1 on success, 0 otherwise.
 */
static int
_picture_cache_add_picture(Picture *picture)
{
   Picture_Cache *cache;
   int th_w, th_h;

   cache = DEVIANM->picture_cache;
   th_w = DEVIANM->conf->data_picture_thumb_default_size;
   th_h = DEVIANM->conf->data_picture_thumb_default_size;

   /* (Load the picture) */
   if (!picture->picture)
      picture->picture = _picture_thumb_get_evas(picture->thumb_path);

   cache->pictures = evas_list_append(cache->pictures, picture);
   picture->cached = 1;

   if (cache->pos == -1)
      cache->pos = evas_list_count(cache->pictures) - 1;

   DDATAC(("ajout ok (%s), pos:%d", picture->picture_description->name, cache->pos));

   return 1;
}

/**
 * Removes a picture from the cache
 *
 * @param picture The picture to remove.
 * If NULL, try to find and remove an unused picture, starting from the top.
 * @return 1 on success, 0 otherwise.
 */
static int
_picture_cache_del_picture(Picture *picture)
{
   Evas_List *l;
   Picture_Cache *cache;

   cache = DEVIANM->picture_cache;

   if (!picture)
     {
        picture = _picture_cache_get_picture_unused();
        if (!picture)
           return 0;
        /* We have a picture to remove =) */
     }

   if (picture->source)
     {
        if (picture->source->picture0 == picture)
           DEVIANF(data_picture_cache_detach) (picture->source, 0);
        else
          {
             if (picture->source->picture1 == picture)
                DEVIANF(data_picture_cache_detach) (picture->source, 1);
             else
               {
                  fprintf(stderr, MODULE_NAME ": SOURCE of a PICTURE doesnt reference the PICTURE !!!\n");
               }
          }
     }

   /* (Unload picture) */
   if (picture->from == DATA_PICTURE_LOCAL)
     {
        if (picture->picture)
           evas_object_del(picture->picture);
        picture->picture = NULL;
     }

   /* Only if picture is before cache->pos, decr cache->pos */
   if (cache->pos != -1)
     {
        l = evas_list_nth_list(cache->pictures, cache->pos);
        if (!evas_list_find(l, picture))
          {
             DDATAC(("retrait %s pos --", picture->picture_description->name));
             cache->pos--;
          }
     }

   cache->pictures = evas_list_remove(cache->pictures, picture);
   picture->cached = 0;

   /* If the pictures pointed by cache->pos is no more valid, set 'no new pics to display' */
   if (cache->pos != -1)
     {
        l = evas_list_nth_list(cache->pictures, cache->pos);
        if (!l)
          {
             DDATAC(("retrait %s was last, pos set to -1", picture->picture_description->name));
             cache->pos = -1;
          }
     }

   DDATAC(("retrait ok (%s), pos %d", picture->picture_description->name, cache->pos));

   return 1;
}

/**
 * Fills the cache
 *
 * Fill the cache to CACHE_SIZE() if possible.
 * If max is reached, try to remove unused old pictures and go again
 * @return Number of pictures added
 */
static int
_picture_cache_fill(void)
{
   Picture_Cache *cache;
   Picture *picture = NULL;
   int new;

   cache = DEVIANM->picture_cache;
   new = 0;

   DDATAC(("fill begin"));

   /* If cache is too big, reduce it */
   while (evas_list_count(cache->pictures) > CACHE_SIZE())
     {
        DDATAC(("cache too big, delete a pic"));
        if (!_picture_cache_del_picture(NULL))
          {
             fprintf(stderr, MODULE_NAME ": !!! Cache too big but cant delete picture ... NEED TO BE FIXED !!!\n");
             break;
          }
     }

   /* Add pictures while we have one, and if max reached, we can del one */
   while ((picture = _picture_list_get_picture(NULL)))
     {
        if (evas_list_count(cache->pictures) == CACHE_SIZE())
          {
             if (!_picture_cache_del_picture(NULL))
                break;
          }
        _picture_cache_add_picture(picture);
        picture = NULL;
        new++;
     }

   DDATAC(("fill end"));

   return new;
}

/**
 * Free a picture
 *
 * Remove it from the cache, a list, and from memory
 */
static int
_picture_free(Picture *picture, int force, int force_now)
{
   Evas_List *l;

   DDATA(("Free picture %s beginf (%d %d)", picture->picture_description->name, force, force_now));

   if (picture->source)
     {
        if (!force)
           return 0;

        /* If not now, only mark as delete, and picture will be deleted
         * on next picture change */
        if (!force_now)
          {
             picture->delete = 1;
             return 1;
          }
     }

   if (picture->cached)
      _picture_cache_del_picture(picture);

   if (picture->picture)
      evas_object_del(picture->picture);

   if (picture->sources_histo)
     {
        Source_Picture *source;

        for (l = picture->sources_histo; l; l = evas_list_next(l))
          {
             source = evas_list_data(l);
             DEVIANF(source_picture_histo_picture_detach) (source, picture);
          }
     }

   if (picture->path)
      evas_stringshare_del(picture->path);
   if (picture->thumb_path)
      E_FREE(picture->thumb_path);

   if (picture->picture_description->name)
      evas_stringshare_del(picture->picture_description->name);
   if (picture->picture_description->author_name)
      E_FREE(picture->picture_description->author_name);
   if (picture->picture_description->where_from)
      E_FREE(picture->picture_description->where_from);
   if (picture->picture_description->date)
      E_FREE(picture->picture_description->date);
   if (picture->picture_description->comments)
      E_FREE(picture->picture_description->comments);
   E_FREE(picture->picture_description);

   if (picture->from == DATA_PICTURE_LOCAL)
     {
        DEVIANM->picture_list_local->pictures = evas_list_remove(DEVIANM->picture_list_local->pictures, picture);
     }
   else
     {
        fprintf(stderr, "DEL FROM NET !!!!!!\n");
        DEVIANM->picture_list_net->pictures = evas_list_remove(DEVIANM->picture_list_net->pictures, picture);
     }

   DDATA(("Picture free ok (%p, %d in list)", picture, evas_list_count(DEVIANM->picture_list_local->pictures)));

   E_FREE(picture);

   return 1;
}

static void
_picture_local_thumb_cb(Evas_Object *obj, void *data)
{
   Picture *picture;
   Picture_List_Local *list;

   if (!DEVIANM || !data)
      return;

   list = DEVIANM->picture_list_local;
   picture = data;

   DDATA(("back from thumb generation of %s", picture->picture_description->name));

   if (ecore_file_exists(picture->thumb_path))
     {
        e_thumb_geometry_get(picture->thumb_path, &picture->original_w, &picture->original_h, 1);
        picture->thumbed = 1;
        DDATA(("thumb generated %dx%d", picture->original_w, picture->original_h));
        list->pictures = evas_list_append(list->pictures, picture);
        list->nb_pictures_waiting--;

        /* If the pic is loaded, remove it, we dont want it !
         * Moreover it does memleak */
        if (picture->picture)
          {
             evas_object_del(picture->picture);
             picture->picture = NULL;
          }
     }
   else
     {
        _picture_free(picture, 1, 1);
     }
}

static Evas_Object *
_picture_thumb_get_evas(char *thumb)
{
   Eet_File *ef;
   Evas_Object *im = NULL;
   Evas_Coord sw, sh;

   ef = eet_open(thumb, EET_FILE_MODE_READ);
   if (!ef)
     {
        eet_close(ef);
        return NULL;
     }

   im = e_icon_add(DEVIANM->container->bg_evas);
   e_icon_file_key_set(im, thumb, "/thumbnail/data");

   e_icon_size_get(im, &sw, &sh);
   evas_object_resize(im, sw, sh);
   e_icon_fill_inside_set(im, 1);

   eet_close(ef);

   return im;
}
#endif
