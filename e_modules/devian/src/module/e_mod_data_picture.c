#include "dEvian.h"

#ifdef HAVE_PICTURE

#define CACHE_SIZE() (DEVIANM->conf->data_picture_cache_size + evas_list_count(DEVIANM->devians) * DEVIANM->conf->data_picture_cache_size)
#define POPUP_SHOW_LOCAL_LOADING(list, buf, dtime) \
list->loader.popup = DEVIANF(popup_warn_add) (POPUP_WARN_TYPE_INFO, buf, \
                                            list->loader.popup, 0, \
                                            DATA_PICTURE_LOADER_POPUP_TIME + dtime, \
                                            NULL, _popup_loader_close, _popup_desactivate);
#define POPUP_SHOW_LOCAL_THUMBNAILING(list, buf, dtime) \
list->thumb_popup = DEVIANF(popup_warn_add) (POPUP_WARN_TYPE_INFO, buf, \
                                            list->thumb_popup, 0, \
                                            DATA_PICTURE_THUMB_POPUP_TIME + dtime, \
                                            NULL, _popup_thumb_close, _popup_desactivate);

static int _picture_list_local_add_dir(const char *dir);
static Picture *_picture_list_get_picture(Source_Picture *client, int picture_type);

static Picture *_picture_cache_get_picture_unused(void);
static int _picture_cache_add_picture(Picture *picture);
static int _picture_cache_del_picture(Picture *picture);
static int _picture_cache_fill(int type, Source_Picture *client);

static int _picture_free(Picture *picture, int force, int force_now);
static void _picture_local_thumb_cb(Evas_Object *obj, void *data);
static void _picture_local_thumb_stop(void);
static Evas_Object *_picture_thumb_get_evas(char *thumb);

static int _load_cb_fill(void *data, int type, void *event);
static int _load_local_idler(void *data);
static void _load_local_idler_stop(void);

static int _popup_loader_close(Popup_Warn *popw, void *data);
static int _popup_thumb_close(Popup_Warn *popw, void *data);
static void _popup_desactivate(Popup_Warn *popw, void *data);


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
	list->loader_ev.id = ecore_event_type_new();
        DEVIANM->picture_list_local = list;
     }
   else
     list = DEVIANM->picture_list_local;

   /* load pictures */
   if (DEVIANM->conf->sources_picture_show_devian_pics)
     _picture_list_local_add_dir(e_module_dir_get(DEVIANM->module));
   _picture_list_local_add_dir(DEVIANM->conf->sources_picture_data_import_dir);

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

   _picture_local_thumb_stop();
   
   _load_local_idler_stop();

   /* remove pictures */
   for (l = list->pictures; l; l = evas_list_next(l))
     {
        picture = evas_list_data(l);
        _picture_free(picture, 1, 1);
     }
   evas_list_free(l);

   E_FREE(DEVIANM->picture_list_local);
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

   _picture_local_thumb_stop();

   _load_local_idler_stop();

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
   cache->pos = -1;
   DEVIANM->picture_cache = cache;

   _picture_cache_fill(DATA_PICTURE_BOTH, NULL);

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
   E_FREE(DEVIANM->picture_cache);
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

   /* if old pictures are still attached, abord */
   if (!edje_part && source->picture0)
     return NULL;
   if (edje_part && source->picture1)
     return NULL;

   if (histo_nb || source->histo_pos)
     {
        int new_pos, n;

        /* get a picture in the histo */

        new_pos = source->histo_pos + histo_nb;
        n = new_pos;
        /* select the picture if unused and not going to be deleted */
        do
          {
             if (n < 0)
               {
                  /* can't get a picture in historic ? So get a new one ! */
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

        /* add it to the cache if wasnt */
	if (!picture->picture)
          if (!_picture_cache_add_picture(picture))
            return NULL;

        /* update the source's histo position */
        source->histo_pos = new_pos;
     }
   else
     {
        /* get a picture in the cache */

        /* need to fill the cache ? */
        if (cache->pos == -1)
          _picture_cache_fill(DATA_PICTURE_BOTH, source);
        else if (cache->pos >
                 (evas_list_count(cache->pictures) - (int)(evas_list_count(cache->pictures) / 4)))
          _picture_cache_fill(DATA_PICTURE_BOTH, source);

        /* get the next picture and change next picture */
        if (cache->pos != -1)
          {
             picture = evas_list_nth(cache->pictures, cache->pos);
             cache->pos++;
             if (cache->pos > (evas_list_count(cache->pictures) - 1))
               cache->pos = -1;        /* overflow -> no more pictures avalaible ! */
             /* if picture isnt good, retry */
             if (picture->source || picture->delete)
               return DEVIANF(data_picture_cache_attach) (source, edje_part, 0);
          }
        else
          {
             picture = _picture_cache_get_picture_unused();
             if (!picture)
               {
                  /* put the devian in the queue to be warned when new pictures
                   * will be added */
		   
                  return NULL;
               }
          }

        /* attach the picture to the source's historic
	 * and attach the source to the picture histo's list */
        DEVIANF(source_picture_histo_picture_attach) (source, picture);
     }

   /* attach the picture to the source */
   picture->source = source;
   if (!edje_part)
     source->picture0 = picture;
   else
     source->picture1 = picture;
   cache->nb_attached++;
   
   DDATAPICC(("attach ok (%s, %p), pos: %d", picture->picture_description->name, picture, cache->pos));

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
             DDATAPICC(("BAD BAD BAD in cache detach"));
          }
     }

   if (!picture)
     return;

   /* detach source from picture */
   picture->source = NULL;

   /* picture needs to be deleted ? */
   if (picture->delete)
     {
        if (picture->from == DATA_PICTURE_LOCAL)
          _picture_free(picture, 1, 1);
     }

   /* detach picture from source */
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
     DDATAPICC(("detach ok (%s)", picture->picture_description->name));
   else
     DDATAPICC(("detach ok (-null-)"));
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

   fichier = strrchr(path, '/');
   if (fichier)
     fichier++;
   else
     fichier = path;

   ext = strrchr(fichier, '.');
   if (ext)
   {
      ext_l = strlen(ext);
      name_l = strlen(fichier) - ext_l;
   }
   else
     name_l = strlen(fichier);
   
   strncpy(name, fichier, name_l);
   name[name_l] = '\0';

   //return (char *)DEVIANF(ss_utf8_add) (name, 0);
   return (char *)evas_stringshare_add(name);
}


/* PRIVATE FUNCTIONS */

static int
_picture_list_local_add_dir(const char *dir)
{
   Picture_List_Local *list;
   char buf[4096];

   list = DEVIANM->picture_list_local;

   /* checks */
   if (!ecore_file_is_dir(dir))
     {
        snprintf(buf, sizeof(buf),
                 _("<hilight>Directory %s doesnt exists.</hilight><br><br>"
                   "To import pictures, you have to put them"
                   "in the folder you set in main configuration panel<br>"
                   "They can be jpeg or png<br><br>"
                   "After import, if you can remove these files and the pictures still can<br>"
                   "be viewed, but you wont be able to set them as wallpaper anymore<br><br>"), dir);
        e_module_dialog_show(_(MODULE_NAME " Module Error"), buf);
        return 0;
     }

   DDATAPIC(("Going to load %s", dir));
   list->loader.path = evas_list_append(list->loader.path, strdup(dir));

   if (!list->loader.idler)
     {
        list->loader.idler = ecore_idler_add(_load_local_idler, NULL);
     }

   return 1;
}

/**
 * Get a picture in a list
 *
 * @param client The picture source wich ask for pictures. Can be NULL.
 * @param picture_type Where to get the picture. If -1, will take the pic in the list indicated by default location
 * @return the picture
 */
static Picture *
_picture_list_get_picture(Source_Picture *client, int picture_type)
{
   Picture *picture;
   Evas_List *l, *was_first;
   Evas_List *pictures_list;

   picture = NULL;

   DDATAPICC(("Trying to get a picture on %d", picture_type));

   switch (picture_type)
     {
     case DATA_PICTURE_LOCAL:
        pictures_list = DEVIANM->picture_list_local->pictures;
        break;

     case DATA_PICTURE_NET:
        pictures_list = DEVIANM->picture_list_net->pictures;
        break;

     case -1:
        switch (DEVIANM->conf->sources_picture_default_location)
          {
          case DATA_PICTURE_LOCAL:
             pictures_list = DEVIANM->picture_list_local->pictures;
	     picture_type = DATA_PICTURE_LOCAL;
             break;
          case DATA_PICTURE_NET:
             pictures_list = DEVIANM->picture_list_net->pictures;
	     picture_type = DATA_PICTURE_NET;
             break;
          case DATA_PICTURE_BOTH:
             {
                /* random between local and net. If one doesnt work, try the other one */
                int i;

                i = rand() % 2;
                if (!i)
                  picture = _picture_list_get_picture(client, DATA_PICTURE_LOCAL);
                else
                  picture = _picture_list_get_picture(client, DATA_PICTURE_NET);

                if (!picture)
		  {
                     if (i)
                       picture = _picture_list_get_picture(client, DATA_PICTURE_LOCAL);
                     else
                       picture = _picture_list_get_picture(client, DATA_PICTURE_NET);
                  }
                return picture;
             }
          }
	break;
     }

   if (evas_list_count(pictures_list))
     {
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
     }

   DDATAPICC(("Trying to get a picture on %d (bis)", picture_type));

   /* no pictures where found */

   if (client)
     {
        if (!client->load_handler)
          {
             /* add a handler to warn the dEvian when new pictures will arrive */
             switch (picture_type)
               {
               case DATA_PICTURE_LOCAL:
                  {
                     /* local picture event */
                     DEVIANM->picture_list_local->loader_ev.nb_clients++;
                     client->load_handler = ecore_event_handler_add(DEVIANM->picture_list_local->loader_ev.id,
                                                                    _load_cb_fill,
                                                                    client);
                     DDATAPIC(("Loading event SET !"));
                     break;
                  }
		     
               case DATA_PICTURE_NET:
                  {
                     /* net picture event */
                     break;
                  }
               }
          }
     }

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

   DDATAPICC(("Going to try to attach unused picture"));

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
        /* current picture ? */
        if (!picture->source && !picture->delete && picture->thumbed)
          {
             DDATAPICC(("Picture found ! %s", picture->picture_description->name));
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

   /* (load the picture) */
   if (!picture->picture)
     picture->picture = _picture_thumb_get_evas(picture->thumb_path);

   cache->pictures = evas_list_append(cache->pictures, picture);
   picture->cached = 1;

   if (cache->pos == -1)
     cache->pos = evas_list_count(cache->pictures) - 1;

   DDATAPICC(("ajout ok (%s), pos:%d", picture->picture_description->name, cache->pos));

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
        /* we have a picture to remove =) */
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

   /* (unload picture) */
   if (picture->from == DATA_PICTURE_LOCAL)
     {
        if (picture->picture)
          evas_object_del(picture->picture);
        picture->picture = NULL;
     }

   /* only if picture is before cache->pos, decr cache->pos */
   if (cache->pos != -1)
     {
        l = evas_list_nth_list(cache->pictures, cache->pos);
        if (!evas_list_find(l, picture))
          {
             DDATAPICC(("retrait %s pos --", picture->picture_description->name));
             cache->pos--;
          }
     }

   cache->pictures = evas_list_remove(cache->pictures, picture);
   picture->cached = 0;

   /* if the pictures pointed by cache->pos is no more valid, set 'no new pics to display' */
   if (cache->pos != -1)
     {
        l = evas_list_nth_list(cache->pictures, cache->pos);
        if (!l)
          {
             DDATAPICC(("retrait %s was last, pos set to -1", picture->picture_description->name));
             cache->pos = -1;
          }
     }

   DDATAPICC(("retrait ok (%s), pos %d", picture->picture_description->name, cache->pos));

   return 1;
}

/**
 * Fills the cache
 *
 * Fill the cache to CACHE_SIZE() if possible.
 * If max is reached, try to remove unused old pictures and go again
 * @param type Type of the picture requested (LOCAL / NET / BOTH)
 * @return Number of pictures added
 */
static int
_picture_cache_fill(int type, Source_Picture *client)
{
   Picture_Cache *cache;
   Picture *picture = NULL;
   int new;

   cache = DEVIANM->picture_cache;
   new = 0;

   DDATAPICC(("fill begin"));

   /* if cache is too big, reduce it */
   while (evas_list_count(cache->pictures) > CACHE_SIZE())
     {
        DDATAPICC(("cache too big, delete a pic"));
        if (!_picture_cache_del_picture(NULL))
          {
             fprintf(stderr, MODULE_NAME ": !!! Cache too big but cant delete picture ... NEED TO BE FIXED !!!\n");
             break;
          }
     }

   /* add pictures while we have one, and if max reached, we can del one */
   while ((picture = _picture_list_get_picture(client, -1)))
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

   DDATAPICC(("fill end"));

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

   DDATAPIC(("Free picture %s beginf (%d %d)", picture->picture_description->name, force, force_now));

   if (picture->source)
     {
        if (!force)
          return 0;

        /* if not now, only mark as delete, and picture will be deleted
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
     free(picture->thumb_path);

   if (picture->picture_description->name)
     evas_stringshare_del(picture->picture_description->name);
   if (picture->picture_description->author_name)
     free(picture->picture_description->author_name);
   if (picture->picture_description->where_from)
     free(picture->picture_description->where_from);
   if (picture->picture_description->date)
     free(picture->picture_description->date);
   if (picture->picture_description->comments)
     free(picture->picture_description->comments);
   free(picture->picture_description);

   if (picture->from == DATA_PICTURE_LOCAL)
     {
        DEVIANM->picture_list_local->pictures = evas_list_remove(DEVIANM->picture_list_local->pictures, picture);
     }
   else
     {
        fprintf(stderr, "DEL FROM NET !!!!!!\n");
        DEVIANM->picture_list_net->pictures = evas_list_remove(DEVIANM->picture_list_net->pictures, picture);
     }

   DDATAPIC(("Picture free ok (%p, %d in list)", picture, evas_list_count(DEVIANM->picture_list_local->pictures)));

   free(picture);

   return 1;
}

static void
_picture_local_thumb_cb(Evas_Object *obj, void *data)
{
   Picture *picture;
   Picture_List_Local *list;

   if (!DEVIANM || !data)
     return;
   if (!e_module_enabled_get(DEVIANM->module))
     return;

   list = DEVIANM->picture_list_local;

   if (!list->thumb_pictures_nb)
     return;

   picture = data;

   list->thumb_pictures_nb--;
   list->thumb_pictures = evas_list_remove(list->thumb_pictures, picture);

   DDATAPIC(("back from thumb generation of %s", picture->picture_description->name));

   if (ecore_file_exists(picture->thumb_path))
     {
        e_thumb_geometry_get(picture->thumb_path,
                             &picture->original_w, &picture->original_h, 1);
        picture->thumbed = 1;
        DDATAPIC(("thumb generated %dx%d", picture->original_w, picture->original_h));

        list->pictures = evas_list_append(list->pictures, picture);

        /* if the pic is loaded, remove it, we dont want it !
         * moreover it does memleak */
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

   if ((DEVIANM->conf->data_picture_popup_when >= DATA_PICTURE_POPUP_WHEN_SUM) &&
       !list->loader.idler)
     {
        if (!list->thumb_pictures_nb)
          {
             /* Last thumbnailing popup message */
             POPUP_SHOW_LOCAL_THUMBNAILING(list, "Thumbnailing finished :)", 2);
          }
        else
          {
             if (DEVIANM->conf->data_picture_popup_when == DATA_PICTURE_POPUP_WHEN_ALWAYS)
               {
                  /* thumbnailing popup message */
                  if (!(list->thumb_pictures_nb%DATA_PICTURE_THUMB_POPUP_MOD))
                    {
                       char buf[50];
                       
                       snprintf(buf, sizeof(buf), "Still %d pictures to thumbnail",
                                list->thumb_pictures_nb);
                       POPUP_SHOW_LOCAL_THUMBNAILING(list, buf, 0);
                    }
               }
          }
     }

   if (list->loader_ev.nb_clients)
     {
        Picture_Event_List_Fill *ev;
	 
        ev = E_NEW(Picture_Event_List_Fill, 1);
        ev->new = 1;
        ev->type = DATA_PICTURE_LOCAL;
        /* raise event to warn : a picture is here for you ! */
        ecore_event_add(list->loader_ev.id, ev, NULL, NULL);
        DDATAPIC(("Loader event RAISED !"));
     }
}

static void
_picture_local_thumb_stop(void)
{
   Picture_List_Local *list;
   Evas_List *l;
   Picture *picture;

   list = DEVIANM->picture_list_local;

   if (list->thumb_pictures)
     {
        for (l=list->thumb_pictures; l; l=evas_list_next(l))
          {
             picture = evas_list_data(l);
             e_thumb_generate_end(picture->path);
             _picture_free(picture, 1, 1);
          }
        evas_list_free(list->thumb_pictures);
     }
   list->thumb_pictures_nb = 0;
   list->thumb_pictures = NULL;

   if (list->thumb_popup)
     {
        DEVIANF(popup_warn_del) (list->thumb_popup);
        list->thumb_popup = NULL;
     }
   list->thumb_popup_show = 0;
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


static int
_load_cb_fill(void *data, int type, void *event)
{
   Source_Picture *source;
   DEVIANN *devian;
   Picture_Event_List_Fill *ev;

   source = data;
   devian = source->devian;
   ev = event;

   DDATAPIC(("Loader - EVENT -"));

   if (!devian->source_func.refresh(devian, 0))
     return 1;

   /* FIXME: use new from ev, for the moment its always 1 */
   if (ev->type == DATA_PICTURE_LOCAL)
     {
        DEVIANM->picture_list_local->loader_ev.nb_clients--;
        DDATAPIC(("Loader event nb_clients : %d", DEVIANM->picture_list_local->loader_ev.nb_clients));
     }

   ecore_event_handler_del(source->load_handler);
   source->load_handler = NULL;

   return 0;
}

static int
_load_local_idler(void *data)
{
   Picture_List_Local *pl;
   char file[DEVIAN_MAX_PATH];
   char *name;

   pl = DEVIANM->picture_list_local;

   if (!pl->loader.dir)
     {
        if (pl->loader.path)
          {
             Evas_List *l;
             
             pl->loader.dir = strdup((char *)evas_list_data(pl->loader.path));

             l = evas_list_next(pl->loader.path);
             if (!l)
               {
                  for(l=evas_list_last(pl->loader.path); l; l=evas_list_prev(l))
                    {
                       name = evas_list_data(l);
                       free(name);
                    }
                  evas_list_free(pl->loader.path);
                  pl->loader.path = NULL;
               }
             else
               {
                  pl->loader.path = l;
               }
             pl->loader.file = ecore_file_ls(pl->loader.dir);
	     if (!pl->loader.file)
               {
                  E_FREE(pl->loader.dir);
               }
	     else
               {
                  if (ecore_list_is_empty(pl->loader.file))
                    {
                       ecore_list_destroy(pl->loader.file);
                       pl->loader.file = NULL;
                       E_FREE(pl->loader.dir);
                    }
               }
          }
        else
          {
             char buf[50];
             pl->loader.idler = NULL;
             /* last loading popup message */
             if (DEVIANM->conf->data_picture_popup_when >= DATA_PICTURE_POPUP_WHEN_SUM)
               {
                  snprintf(buf, sizeof(buf), "Scan finished : %d pictures found",
                           evas_list_count(pl->pictures) + pl->thumb_pictures_nb);
                  POPUP_SHOW_LOCAL_LOADING(pl, buf, 3);
                  /* tell how much pictures to thumb */
                  if (pl->thumb_pictures_nb)
                    {
                       snprintf(buf, sizeof(buf), "Still %d pictures to thumbnail",
                                pl->thumb_pictures_nb);
                       POPUP_SHOW_LOCAL_THUMBNAILING(pl, buf, 3);
                    }
               }
             return 0;
          }	 
     }

   if (pl->loader.dir)
     {
        name = ecore_list_next(pl->loader.file);
        if (!name)
          {
             ecore_list_destroy(pl->loader.file);
             pl->loader.file = NULL;
             E_FREE(pl->loader.dir);
          }
        else
          {
             int th_w, th_h;
             char *file_tmp, *ext;
             Picture *picture;

             if (!DEVIANM->conf->sources_picture_data_import_hidden)
               if (name[0] == '.')
                 return 1;

             th_w = DEVIANM->conf->data_picture_thumb_default_size;
             th_h = DEVIANM->conf->data_picture_thumb_default_size;

             snprintf(file, sizeof(file), "%s/%s", pl->loader.dir, name);
   
             if ((file_tmp = ecore_file_readlink(file)))
               {
                  name = strdup(ecore_file_get_file(file_tmp));
                  strncpy(file, file_tmp, sizeof(file));
               }

             if (DEVIANM->conf->sources_picture_data_import_recursive)
               {
                  if (ecore_file_is_dir(file))
                    {
                       pl->loader.path = evas_list_append(pl->loader.path, strdup(file));
                       return 1;
                    }
               }
   
             ext = strrchr(name, '.');
             if (!ext)
               return 1;
             if (strcasecmp(ext, ".jpg") && strcasecmp(ext, ".JPG") &&
                 strcasecmp(ext, ".jpeg") && strcasecmp(ext, ".JPEG") &&
                 strcasecmp(ext, ".png") && strcasecmp(ext, ".PNG"))
               return 1;
   
             DDATAPIC(("File %s loading ...", name));
	
             picture = E_NEW(Picture, 1);
             picture->path = (char *)evas_stringshare_add(file);
             picture->thumb_path = e_thumb_file_get(picture->path);
             picture->picture_description = E_NEW(Picture_Infos, 1);
             picture->picture_description->name = DEVIANF(data_picture_get_name_from_path) (name, DATA_PICTURE_INFOS_LEN);
             picture->from = DATA_PICTURE_LOCAL;

             DDATAPIC(("Thumb %s of %s exists ?", picture->thumb_path, picture->path));
             if (e_thumb_exists(picture->path))
               {
                  int w, h;

                  e_thumb_geometry_get(picture->thumb_path, &w, &h, 1);
                  DDATAPIC(("THUMB %dx%d (wanted %dx%d)", w, h, th_w, th_h));
                  if ((th_w > w) && (th_h > h))
                    {
                       /* thumb exists, but regen to new size */
                       int i;

                       i = ecore_file_unlink(picture->thumb_path);
                       DDATAPIC(("File %s thumb exists (%dx%d),  but regen to %dx%d (del old %d)", file, w, h, th_w, th_h, i));
		       pl->thumb_pictures = evas_list_append(pl->thumb_pictures,
                                                             picture);
                       pl->thumb_pictures_nb++;
                       e_thumb_generate_begin(picture->path, th_w, th_h,
                                              DEVIANM->container->bg_evas,
                                              &picture->picture, _picture_local_thumb_cb,
                                              picture);
                    }
                  /* thumb exists and good size */
                  DDATAPIC(("File %s thumb exists and good size, add (%de)", file, evas_list_count(pl->pictures)));
                  picture->thumbed = 1;
                  picture->original_w = w;
                  picture->original_h = h;
                  pl->pictures = evas_list_append(pl->pictures, picture);
               }
             else
               {
                  /* thumb doesnt exists so generate it */
                  DDATAPIC(("File %s thumb doesnt exist, gen %dx%d", file, th_w, th_h));
		  pl->thumb_pictures = evas_list_append(pl->thumb_pictures,
                                                        picture);
                  pl->thumb_pictures_nb++;
                  e_thumb_generate_begin(picture->path, th_w, th_h,
                                         DEVIANM->container->bg_evas,
                                         &picture->picture, _picture_local_thumb_cb,
                                         picture);
               }
	       
          }
     }

   if (DEVIANM->conf->data_picture_popup_when == DATA_PICTURE_POPUP_WHEN_ALWAYS)
     {
        int nb;

        /* loading popup message */        
        nb = evas_list_count(pl->pictures) + pl->thumb_pictures_nb;
        if (nb && ((nb == 1) || !(nb%DATA_PICTURE_LOADER_POPUP_MOD)))
          {
             char buf[50];
             
             if (nb == 1)
               snprintf(buf, sizeof(buf), "Scanning for pictures");
             else
               snprintf(buf, sizeof(buf), "%d pictures found", nb);
             POPUP_SHOW_LOCAL_LOADING(pl, buf, 0);
          }

        /* thumbnailing popup message */
        if (!pl->thumb_popup_show && (pl->thumb_pictures_nb == 1))
          {
             POPUP_SHOW_LOCAL_THUMBNAILING(pl, "Thumbnailing some pictures", 0);
             pl->thumb_popup_show = 1;
          }
     }

   /* thumbnailing message */
   if ((pl->thumb_pictures_nb > 2) &&
       DEVIANM->conf->sources_picture_data_import_thumbs_warn)
     {
        DEVIANM->conf->sources_picture_data_import_thumbs_warn = 0;
        e_module_dialog_show(_(MODULE_NAME " Module Information"),
                             _("<hilight>Creating thumbs</hilight><br><br>"
                               "Some pictures are being thumbed in a <hilight>background task</hilight>.<br>"
                               "It can take a while, but after, loading will be faster :)<br><br>"
                               "Each time wou will load pictures that haven't been loaded in devian before,<br>"
                               "they will be thumbed"));
     }

   /* loader event */
   if (pl->loader_ev.nb_clients)
     {
        Picture_Event_List_Fill *ev;
	 
        ev = E_NEW(Picture_Event_List_Fill, 1);
        ev->new = 1;
        ev->type = DATA_PICTURE_LOCAL;
        /* raise event to warn : a picture is here for you ! */
        ecore_event_add(pl->loader_ev.id, ev, NULL, NULL);
        DDATAPIC(("Loader event RAISED !"));
     }

   return 1;
}

static void
_load_local_idler_stop(void)
{
   Picture_List_Local *list;
   Evas_List *l;
   char *name;

   list = DEVIANM->picture_list_local;

   if (list->loader.idler)
     {
        ecore_idler_del(list->loader.idler);
        list->loader.idler = NULL;
     }
   if (list->loader.path)
     {
        for(l=evas_list_last(list->loader.path); l; l=evas_list_prev(l))
          {
             name = evas_list_data(l);
             free(name);
          }
        evas_list_free(list->loader.path);
        list->loader.path = NULL;
     }
   if (list->loader.dir)
     {
        E_FREE(list->loader.dir);
     }
   if (list->loader.file)
     {
        ecore_list_destroy(list->loader.file);
        list->loader.file = NULL;
     }
   if (list->loader.popup)
     {
        DEVIANF(popup_warn_del) (list->loader.popup);
        list->loader.popup = NULL;
     }
}

static int
_popup_loader_close(Popup_Warn *popw, void *data)
{
   DEVIANM->picture_list_local->loader.popup = NULL;
   return 1;
}

static int
_popup_thumb_close(Popup_Warn *popw, void *data)
{
   DEVIANM->picture_list_local->thumb_popup = NULL;
   return 1;
}

static void
_popup_desactivate(Popup_Warn *popw, void *data)
{
   if (DEVIANM->conf->data_picture_popup_when != DATA_PICTURE_POPUP_WHEN_NEVER)
     DEVIANM->conf->data_picture_popup_when--;
}
	 
#endif
