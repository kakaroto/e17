#include "Photo.h"

#define CACHE_SIZE() (2 + evas_list_count(DEVIANM->devians)*2)

#define POPUP_LOADING(list, buf, dtime)                                \
if (list->loader.popup)                                                \
  photo_popup_warn_del(list->loader.popup);                            \
list->loader.popup =                                                   \
  photo_popup_warn_add(POPUP_WARN_TYPE_NEWS, buf,                      \
                       PICTURE_LOCAL_POPUP_LOADER_TIME + dtime,        \
                       _popup_loader_close, _popup_loader_desactivate, \
		       NULL);

#define POPUP_THUMBNAILING(list, buf, dtime)                         \
if (list->thumb.popup)                                               \
  photo_popup_warn_del(list->thumb.popup);                            \
list->thumb.popup =                                                  \
  photo_popup_warn_add(POPUP_WARN_TYPE_NEWS, buf,                    \
                       PICTURE_LOCAL_POPUP_THUMB_TIME + dtime,       \
                       _popup_thumb_close, _popup_thumb_desactivate, \
                       NULL);


typedef struct _Picture_Local_List Picture_Local_List;

struct _Picture_Local_List
{
   Evas_List *pictures;

   /* thumb */
  struct
  {
    Evas_List  *pictures;
    Popup_Warn *popup;
    int         popup_show;
  } thumb;

   /* ecore idler to load in background */
   struct
   {
      Ecore_Idler *idler;
      Popup_Warn  *popup;

      Picture_Local_Dir *current_dir;

      Evas_List  *dirs;
      Ecore_List *file;
   } loader;

   /* event to warn photo items wich are waiting for pictures */
   struct
   {
      int id;
      int nb_clients;
   } loader_ev;
};

static Picture_Local_List *pictures_local;


static int  _picture_new(char *name);
static void _pictures_old_del(int force, int force_now);

static int  _load_idler(void *data);
static void _load_idler_stop(void);
static int  _load_cb_ev_fill(void *data, int type, void *event);

static void _thumb_generate_cb(Evas_Object *obj, void *data);
static void _thumb_generate_stop(void);

static int  _popup_loader_close(Popup_Warn *popw, void *data);
static void _popup_loader_desactivate(Popup_Warn *popw, void *data);
static int  _popup_thumb_close(Popup_Warn *popw, void *data);
static void _popup_thumb_desactivate(Popup_Warn *popw, void *data);


/*
 * Public functions
 */

int photo_picture_local_init(void)
{
   Picture_Local_List *list;

   list = E_NEW(Picture_Local_List, 1);
   list->loader_ev.id = ecore_event_type_new();
   pictures_local = list;

   photo_picture_local_load_start();

   return 1;
}

void photo_picture_local_shutdown(void)
{
   photo_picture_local_load_stop();
   
   _pictures_old_del(1, 1);

   evas_list_free(pictures_local->pictures);
   pictures_local = NULL;
}

void photo_picture_local_load_start(void)
{
   Evas_List *l;

   photo_picture_local_load_stop();

   /* del old pictures */
   _pictures_old_del(1, 0);

   /* set all directories to not loaded */
   for (l=photo->config->local.dirs; l; l=evas_list_next(l))
     {
        Picture_Local_Dir *d;
        d = evas_list_data(l);
        d->state = PICTURE_LOCAL_DIR_NOT_LOADED;
     }

   /* load pictures */
   pictures_local->loader.idler = ecore_idler_add(_load_idler, NULL);

   if (photo->config_dialog)
     photo_config_dialog_refresh_local_load();
}

void photo_picture_local_load_stop(void)
{
   _load_idler_stop();
   _thumb_generate_stop();

   if (photo->config_dialog)
     {
        photo_config_dialog_refresh_local_infos();
        photo_config_dialog_refresh_local_dirs();
        photo_config_dialog_refresh_local_load();
     }
}

int photo_picture_local_load_state_get(void)
{
   if (pictures_local->thumb.pictures ||
       pictures_local->loader.idler)
     return 1;

   return 0;
}

Picture *photo_picture_local_get(int position)
{
   Picture *picture;
   Evas_List *l, *was_first;
   Picture_Local_List *pl;

   pl = pictures_local;

   DPICL(("Trying to get a picture, position = %d", position));

   if (!evas_list_count(pl->pictures))
     return NULL;

   if (position != PICTURE_LOCAL_GET_RANDOM)
     {
        /* get the given picture */
        l = evas_list_nth_list(pl->pictures, position);
        picture = evas_list_data(l);
     }
   else
     {
        /* get a random picture */
        l = evas_list_nth_list(pl->pictures,
                               rand() % evas_list_count(pl->pictures));
        was_first = l;
        do
          {
             DD(("- Search -"));
             picture = evas_list_data(l);
             if (!picture->pi && !picture->delete)
               return picture;
             l = evas_list_next(l);
             if (!l) l = pl->pictures;
          } while (l != was_first);
        picture = NULL;
     }
  
   return picture;
}

int photo_picture_local_loaded_nb_get(void)
{
   return evas_list_count(pictures_local->pictures);
}

int photo_picture_local_tothumb_nb_get(void)
{
   return evas_list_count(pictures_local->thumb.pictures);
}

void photo_picture_local_ev_set(Photo_Item *pi)
{
   if (pi->local_ev_fill_handler)
     return;

   DPICL(("Set event fill"));

   pictures_local->loader_ev.nb_clients++;
   pi->local_ev_fill_handler = ecore_event_handler_add(pictures_local->loader_ev.id,
                                                       _load_cb_ev_fill,
                                                       pi);
}

void photo_picture_local_ev_raise(int nb)
{
   Picture_Event_Fill *ev;
	 
   if (!pictures_local->loader_ev.nb_clients) return;

   ev = E_NEW(Picture_Event_Fill, 1);
   ev->new = nb;
   ev->type = PICTURE_LOCAL;
   /* raise event to warn clients : a picture is here for you ! */
   ecore_event_add(pictures_local->loader_ev.id, ev, NULL, NULL);
   DPICL(("Loader event RAISED !"));
}

Picture_Local_Dir *photo_picture_local_dir_new(char *path, int recursive, int read_hidden)
{
   Picture_Local_Dir *dir;

   if (!ecore_file_is_dir(path))
     {
        char buf[4096];
        snprintf(buf, sizeof(buf),
                 _("<hilight>Directory %s doesnt exists.</hilight><br><br>"
                   "You can change the picture's folders in main configuration panel<br>"
                   "They can be jpeg or png<br><br>"
                   "After import, if you can remove these files and the pictures still can<br>"
                   "be viewed, but you wont be able to set them as wallpaper anymore<br><br>"),
                 path);
        e_module_dialog_show(photo->module, _("Photo Module Error"), buf);
        return NULL;
     }

   dir = E_NEW(Picture_Local_Dir, 1);
   dir->path = evas_stringshare_add(path);
   dir->recursive = recursive;
   dir->read_hidden = read_hidden;
   dir->state = PICTURE_LOCAL_DIR_NOT_LOADED;

   DPICL(("New directory created : %s", path));

   return dir;
}

void photo_picture_local_dir_free(Picture_Local_Dir *dir, int del_dialog)
{
   if (dir->config_dialog && del_dialog)
     photo_config_dialog_dir_hide(dir);
   evas_stringshare_del(dir->path);
   free(dir);
}


/*
 * Private functions
 */

static int
_picture_new(char *name)
{
   Picture_Local_List *pl;
   Picture *picture;
   char file[4096];
   int th_w, th_h;
   char *file_tmp, *ext;

   pl = pictures_local;

   file_tmp = evas_list_data(pl->loader.dirs);
   snprintf(file, sizeof(file),
            "%s/%s", file_tmp, name);

   DD(("File %s", file));

   if (!pl->loader.current_dir->read_hidden && (file[0] == '.'))
     return 0;

   th_w = photo->config->pictures_thumb_size;
   th_h = photo->config->pictures_thumb_size;
   
   if ((file_tmp = ecore_file_readlink(file)))
     {
        name = strdup(ecore_file_get_file(file_tmp));
        strncpy(file, file_tmp, sizeof(file));
     }
      
   if (pl->loader.current_dir->recursive &&
       ecore_file_is_dir(file))
     {
        pl->loader.dirs = evas_list_append(pl->loader.dirs, strdup(file));
        DPICL(("added %s to loader dirs", file));
        return 0;
     }
      
   ext = strrchr(name, '.');
   if (!ext)
     return 0;
   if (strcasecmp(ext, ".jpg") && strcasecmp(ext, ".JPG") &&
       strcasecmp(ext, ".jpeg") && strcasecmp(ext, ".JPEG") &&
       strcasecmp(ext, ".png") && strcasecmp(ext, ".PNG"))
     return 0;
      
   DPICL(("File %s loading ...", file));
      
   picture = E_NEW(Picture, 1);
   picture->path = evas_stringshare_add(file);
   file_tmp = e_thumb_file_get((char *)picture->path);
   picture->thumb_path = evas_stringshare_add(file_tmp);
   free(file_tmp);
   picture->infos.name = photo_picture_name_get(name);
   picture->from = PICTURE_LOCAL;

   DPICL(("Thumb %s of %s exists ?", picture->thumb_path, picture->path));
   if (e_thumb_exists((char *)picture->path))
     {
        int w, h;

        e_thumb_geometry_get((char *)picture->thumb_path, &w, &h, 1);
        DPICL(("THUMB %dx%d (wanted %dx%d)", w, h, th_w, th_h));
        if ((th_w > w) && (th_h > h))
          {
             /* thumb exists, but regen to new size */
             int i;

             i = ecore_file_unlink(picture->thumb_path);
             DPICL(("File %s thumb exists (%dx%d),  but regen to %dx%d (del old %d)", file, w, h, th_w, th_h, i));
             pl->thumb.pictures = evas_list_append(pl->thumb.pictures,
                                                   picture);
             e_thumb_generate_begin((char *)picture->path, th_w, th_h,
                                    photo->e_evas,
                                    &picture->picture, _thumb_generate_cb,
                                    picture);
          }
        else
          {
             /* thumb exists and good size */
             DPICL(("File %s thumb exists and good size, add (%de)", file, evas_list_count(pl->pictures)));
             picture->original_w = w;
             picture->original_h = h;
             pl->pictures = evas_list_append(pl->pictures, picture);
             if (pl->loader_ev.nb_clients)
               photo_picture_local_ev_raise(1);
             if (photo->config_dialog &&
                 !(evas_list_count(pl->pictures)%100))
               photo_config_dialog_refresh_local_infos();
          }
     }
   else
     {
        /* thumb doesnt exists so generate it */
        DPICL(("File %s thumb doesnt exist, gen %dx%d", file, th_w, th_h));
        pl->thumb.pictures = evas_list_append(pl->thumb.pictures,
                                              picture);
        e_thumb_generate_begin((char *)picture->path, th_w, th_h,
                               photo->e_evas,
                               &picture->picture, _thumb_generate_cb,
                               picture);
     }

   return 1;
}

static void
_pictures_old_del(int force, int force_now)
{
   Picture *p;
   int no = 0;
   
   while ( (p = evas_list_nth(pictures_local->pictures, no)) )
     {
        if (photo_picture_free(p, force, force_now))
          pictures_local->pictures = evas_list_remove(pictures_local->pictures, p);
        else
          no++;
     }
}

static int
_load_idler(void *data)
{
   Picture_Local_List *pl;
   Picture_Local_Dir *d;
   Evas_List *l;
   char *name;

   pl = pictures_local;

   DD(("o"));

   /* no more dirs in the current_dir */
   if (!evas_list_count(pl->loader.dirs))
     {
        /* find a dir to load in user dir list */
        if (pl->loader.current_dir)
          {
             pl->loader.current_dir->state = PICTURE_LOCAL_DIR_LOADED;
             pl->loader.current_dir = NULL;
	     if (photo->config_dialog)
	       photo_config_dialog_refresh_local_dirs();
          }
        DD(("oo"));
        for (l=photo->config->local.dirs; l; l=evas_list_next(l))
          {
             d = evas_list_data(l);
             DD(("ooo"));
             if (d->state == PICTURE_LOCAL_DIR_NOT_LOADED)
               {
                  d->state = PICTURE_LOCAL_DIR_LOADING;
                  pl->loader.current_dir = d;
                  pl->loader.dirs = evas_list_append(pl->loader.dirs,
                                                     strdup(d->path));
		  if (pl->loader.file && ecore_list_is_empty(pl->loader.file))
		    ecore_list_destroy(pl->loader.file);
		  pl->loader.file = NULL;
                  DPICL(("Going to read %s", d->path));
		  if (photo->config_dialog)
		    photo_config_dialog_refresh_local_dirs();
                  return 1;
               }
          }
        /* no more directories to load */
        if (!pl->loader.current_dir)
          {
             if (pl->loader.file)
               ecore_list_destroy(pl->loader.file);
             pl->loader.file = NULL;
             pl->loader.idler = NULL;
             if (photo->config_dialog)
               {
                  photo_config_dialog_refresh_local_infos();
                  photo_config_dialog_refresh_local_load();
               }
	     /* last loading popup message */
             if (photo->config->local.popup >= PICTURE_LOCAL_POPUP_SUM)
               {
		 char buf[50];
		 int thumb_nb = evas_list_count(pl->thumb.pictures);
		 snprintf(buf, sizeof(buf), "Scan finished : %d pictures found",
			  evas_list_count(pl->pictures) + thumb_nb);
		 POPUP_LOADING(pl, buf, 3);
		 /* tell how much pictures to thumb */
		 if (thumb_nb)
		   {
		     snprintf(buf, sizeof(buf), "Still %d pictures to thumbnail",
			      thumb_nb);
		     POPUP_THUMBNAILING(pl, buf, 3);
		   }
               }
             return 0;
          }
     }

   /* first dir list */
   if ( !pl->loader.file )
     pl->loader.file = ecore_file_ls(evas_list_data(pl->loader.dirs));

   /* no more files in the current loader.dirs item */
   if ( !(name = ecore_list_next(pl->loader.file)) )
     {
        DD(("removing %s", (char *)evas_list_data(pl->loader.dirs)));
        /* go to next dir */
	ecore_list_destroy(pl->loader.file);
	pl->loader.file = NULL;
        free(evas_list_data(pl->loader.dirs));
        pl->loader.dirs = evas_list_remove_list(pl->loader.dirs,
                                                pl->loader.dirs);
        if (!evas_list_count(pl->loader.dirs))
          return 1;
        /* list the new dir */
        pl->loader.file = ecore_file_ls(evas_list_data(pl->loader.dirs));
        return 1;
     }

   /* create the picture */
   if (_picture_new(name))
     {
        /* popups */
        if (photo->config->local.popup == PICTURE_LOCAL_POPUP_ALWAYS)
          {
             int nb;
             
             /* loading popup message */        
             nb = evas_list_count(pl->pictures) + evas_list_count(pl->thumb.pictures);
             if (nb && ((nb == 1) || !(nb%PICTURE_LOCAL_POPUP_LOADER_MOD)))
               {
                  char buf[50];
                  
                  if (nb == 1)
                    snprintf(buf, sizeof(buf), "Scanning for pictures");
                  else
                    snprintf(buf, sizeof(buf), "%d pictures found", nb);
                  POPUP_LOADING(pl, buf, 0);
               }
             
             /* thumbnailing popup message */
             if (pl->thumb.popup_show && (evas_list_count(pl->thumb.pictures) == 1))
               {
                  POPUP_THUMBNAILING(pl, "Thumbnailing some pictures", 0);
                  pl->thumb.popup_show = 0;
               }
          }
        
        /* thumbnailing message */
        if (photo->config->local.thumb_msg &&
            (evas_list_count(pl->thumb.pictures) > 2))
          {
             photo->config->local.thumb_msg = 0;
             e_module_dialog_show(photo->module, _("Photo Module Information"),
                                  _("<hilight>Creating thumbs</hilight><br><br>"
                                    "Some pictures are being thumbed in a <hilight>background task</hilight>.<br>"
                                    "It can take a while, but after, loading will be faster :)<br><br>"
                                    "Each time wou will load pictures that haven't been loaded in devian before,<br>"
                                    "they will be thumbed"));
          }
     }

   return 1;
}

static void
_load_idler_stop(void)
{
   Picture_Local_List *pl;

   pl = pictures_local;

   DPICL(("Idler stop !"));

   if (pl->loader.idler)
     {
        ecore_idler_del(pl->loader.idler);
        pl->loader.idler = NULL;
     }

   if (pl->loader.popup)
     {
        photo_popup_warn_del(pl->loader.popup);
        pl->loader.popup = NULL;
     }

   if (pl->loader.current_dir)
     pl->loader.current_dir->state = PICTURE_LOCAL_DIR_NOT_LOADED;
   pl->loader.current_dir = NULL;

   if (pl->loader.dirs)
     {
	Evas_List *l;

        for(l=pl->loader.dirs; l; l=evas_list_next(l))
          {
	     char *name;
             name = evas_list_data(l);
             free(name);
          }
        evas_list_free(pl->loader.dirs);
        pl->loader.dirs = NULL;
     }

   if (pl->loader.file)
     {
        ecore_list_destroy(pl->loader.file);
        pl->loader.file = NULL;
     }
}

static int
_load_cb_ev_fill(void *data, int type, void *event)
{
   Photo_Item *pi;
   Picture_Event_Fill *ev;

   pi = data;
   ev = event;

   DPICL(("Loader - EVENT -"));

   ecore_event_handler_del(pi->local_ev_fill_handler);
   pi->local_ev_fill_handler = NULL;

   photo_item_action_change(pi, 1);

   pictures_local->loader_ev.nb_clients--;
   DPICL(("Loader event nb_clients : %d", pictures_local->loader_ev.nb_clients));

   ev->new--;
   if (!ev->new)
     return 0;
 
   return 1;
}

static void
_thumb_generate_cb(Evas_Object *obj, void *data)
{
   Picture *picture;
   Picture_Local_List *pl;

   if (!photo || !data)
     return;
   if (!e_module_enabled_get(photo->module))
     return;

   pl = pictures_local;

   if (!evas_list_count(pl->thumb.pictures))
     return;

   picture = data;

   pl->thumb.pictures = evas_list_remove(pl->thumb.pictures, picture);
   if (!evas_list_count(pl->thumb.pictures))
     {
        pl->thumb.pictures = NULL;
        if (photo->config_dialog)
          photo_config_dialog_refresh_local_load();
     }

   DPICL(("back from thumb generation of %s", picture->infos.name));

   if (ecore_file_exists(picture->thumb_path))
     {
        e_thumb_geometry_get((char *)picture->thumb_path,
                             &picture->original_w, &picture->original_h, 1);
        DPICL(("thumb generated %dx%d", picture->original_w, picture->original_h));

        pl->pictures = evas_list_append(pl->pictures, picture);

        /* if the pic is loaded, remove it, we dont want it !
         * moreover it does memleak */
        if (picture->picture)
          {
             evas_object_del(picture->picture);
             picture->picture = NULL;
          }

        if (photo->config_dialog)
          photo_config_dialog_refresh_local_infos();
     }
   else
     {
        photo_picture_free(picture, 1, 1);
     }

   if ((photo->config->local.popup >= PICTURE_LOCAL_POPUP_SUM) &&
       !pl->loader.idler)
     {
        if (!evas_list_count(pl->thumb.pictures))
          {
             /* Last thumbnailing popup message */
             POPUP_THUMBNAILING(pl, "Thumbnailing finished :)", 2);
          }
        else
          {
             if (photo->config->local.popup == PICTURE_LOCAL_POPUP_ALWAYS)
               {
                  /* thumbnailing popup message */
                  if (!(evas_list_count(pl->thumb.pictures)%PICTURE_LOCAL_POPUP_THUMB_MOD))
                    {
                       char buf[50];
                       
                       snprintf(buf, sizeof(buf), "Still %d pictures to thumbnail",
                                evas_list_count(pl->thumb.pictures));
                       POPUP_THUMBNAILING(pl, buf, 0);
                    }
               }
          }
     }

   if (pl->loader_ev.nb_clients)
     photo_picture_local_ev_raise(1);
}

static void
_thumb_generate_stop(void)
{
   Picture_Local_List *pl;
   Evas_List *l;
   Picture *picture;

   pl = pictures_local;

   if (pl->thumb.pictures)
     {
        for (l=pl->thumb.pictures; l; l=evas_list_next(l))
          {
             picture = evas_list_data(l);
             e_thumb_generate_end((char *)picture->path);
             photo_picture_free(picture, 1, 1);
          }
        evas_list_free(pl->thumb.pictures);
     }

   pl->thumb.pictures = NULL;

   if (pl->thumb.popup)
     {
        photo_popup_warn_del(pl->thumb.popup);
        pl->thumb.popup = NULL;
     }

   pl->thumb.popup_show = 1;
}

static int
_popup_loader_close(Popup_Warn *popw, void *data)
{
   pictures_local->loader.popup = NULL;
   return 1;
}

static void
_popup_loader_desactivate(Popup_Warn *popw, void *data)
{
   if (photo->config->local.popup != PICTURE_LOCAL_POPUP_NEVER)
     photo->config->local.popup--;
   photo_config_save();
}

static int
_popup_thumb_close(Popup_Warn *popw, void *data)
{
   pictures_local->thumb.popup = NULL;
   return 1;
}

static void
_popup_thumb_desactivate(Popup_Warn *popw, void *data)
{
   if (photo->config->local.popup != PICTURE_LOCAL_POPUP_NEVER)
     photo->config->local.popup--;
   photo_config_save();
}
