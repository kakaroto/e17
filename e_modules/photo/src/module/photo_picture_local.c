#include "Photo.h"

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
     int nb;
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


static void _pictures_old_del(int force, int force_now);

static int  _load_idler(void *data);
static void _load_idler_stop(void);
static int  _load_cb_ev_fill(void *data, int type, void *event);

static void _thumb_generate_cb(void *data, Evas_Object *obj, void *event_info);
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

   /* initialise thumbnailer */
   pictures_local->thumb.nb = 0;
   pictures_local->thumb.popup_show = 1;
   pictures_local->thumb.popup = NULL;

   /* initialise and launch loader */
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
   if (pictures_local->thumb.nb ||
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
             if (!picture->pi && !picture->delete_me &&
		 (picture->thumb != PICTURE_THUMB_WAITING))
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
  return (evas_list_count(pictures_local->pictures) -
	  pictures_local->thumb.nb);
}

int photo_picture_local_tothumb_nb_get(void)
{
   return pictures_local->thumb.nb;
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
   Picture *picture;
   char *name, *file_tmp;
   char file[200];

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

		 snprintf(buf, sizeof(buf), "Scan finished : %d pictures found",
			  evas_list_count(pl->pictures) + pl->thumb.nb);
		 POPUP_LOADING(pl, buf, 3);
		 /* tell how much pictures to thumb */
		 if (pl->thumb.nb)
		   {
		     snprintf(buf, sizeof(buf), "Still %d pictures to thumbnail",
			      pl->thumb.nb);
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

   snprintf(file, sizeof(file),
            "%s/%s", (char *)evas_list_data(pl->loader.dirs), name);

   if (!pl->loader.current_dir->read_hidden && (name[0] == '.'))
     return 1;
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
        return 1;
     }

   /* create the picture */
   picture = photo_picture_new(file, name, 1, _thumb_generate_cb);
   if (!picture)
     return 1;

   pl->thumb.nb++;
   pl->pictures = evas_list_append(pl->pictures, picture);

   /* loader popups */
   if (photo->config->local.popup == PICTURE_LOCAL_POPUP_ALWAYS)
     {
        int nb;
        
        /* loading popup message */        
        nb = evas_list_count(pl->pictures) + pl->thumb.nb;
        if (nb && ((nb == 1) || !(nb%PICTURE_LOCAL_POPUP_LOADER_MOD)))
          {
             char buf[50];
             
             if (nb == 1)
               snprintf(buf, sizeof(buf), "Scanning for pictures");
             else
               snprintf(buf, sizeof(buf), "%d pictures found", nb);
             POPUP_LOADING(pl, buf, 0);
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
_thumb_generate_cb(void *data, Evas_Object *obj, void *event_info)
{
   Picture *picture;
   Picture_Local_List *pl;

   if (!photo || !e_module_enabled_get(photo->module))
     return;

   pl = pictures_local;
   picture = data;

   DPICL(("back from thumb generation of %s", picture->infos.name));

   if (!obj)
     {
        DPICL(("generated object is NULL !!"));
        photo_picture_free(picture, 1, 1);
        return;
     }

   evas_object_geometry_get(obj, NULL, NULL,
			    &picture->original_w, &picture->original_h);
   DPICL(("thumb generated %dx%d", picture->original_w, picture->original_h));

   picture->thumb = PICTURE_THUMB_READY;

   pl->thumb.nb--;

   /* popups about thumbnailing */

   /* first thumbnailing popup message */
   if (photo->config->local.popup &&
       pl->thumb.popup_show && (pl->thumb.nb == 1))
     {
        pl->thumb.popup_show = 0;
        POPUP_THUMBNAILING(pl, "Thumbnailing some pictures", 0);
     }
   
   /* thumbnailing message, only one time */
   if (photo->config->local.thumb_msg)
     {
        photo->config->local.thumb_msg = 0;
	photo_config_save();
        e_module_dialog_show(photo->module, _("Photo Module Information"),
                             _("<hilight>Creating thumbs</hilight><br><br>"
                               "Some pictures are being thumbed in a <hilight>background task</hilight>.<br>"
                               "It can take a while, but after, loading will be faster :)<br><br>"
                               "Each time wou will load pictures that haven't been loaded in Photo module before,<br>"
                               "they will be thumbed"));
     }

   /* when still thumbnailing after loading */
   if ((photo->config->local.popup >= PICTURE_LOCAL_POPUP_SUM) &&
       !pl->loader.idler)
     {
        if (!pl->thumb.nb)
          {
             /* Last thumbnailing popup message */
             POPUP_THUMBNAILING(pl, "Thumbnailing finished :)", 2);
          }
        else
          {
             if (photo->config->local.popup == PICTURE_LOCAL_POPUP_ALWAYS)
               {
                  /* thumbnailing popup message */
                  if (!(pl->thumb.nb%PICTURE_LOCAL_POPUP_THUMB_MOD))
                    {
                       char buf[50];
                       
                       snprintf(buf, sizeof(buf), "Still %d pictures to thumbnail",
                                pl->thumb.nb);
                       POPUP_THUMBNAILING(pl, buf, 0);
                    }
               }
          }
     }

   /* refreshes */

   if (!pl->thumb.nb && photo->config_dialog)
     photo_config_dialog_refresh_local_load();

   if (pl->loader_ev.nb_clients)
     photo_picture_local_ev_raise(1);

   if (photo->config_dialog &&
       !(evas_list_count(pl->pictures)%100))
     photo_config_dialog_refresh_local_infos();

   /* if the pic is loaded, remove it, we dont want it */

   if (picture->picture)
     {
        evas_object_del(picture->picture);
        picture->picture = NULL;
     }
}

static void
_thumb_generate_stop(void)
{
   Picture_Local_List *pl;
   Evas_List *l;
   Picture *picture;

   pl = pictures_local;

   if (pl->thumb.nb)
     {
        for (l=pl->pictures; l; l=evas_list_next(l))
          {
             picture = evas_list_data(l);
             if (picture->thumb != PICTURE_THUMB_WAITING)
               continue;

             e_thumb_icon_end(picture->picture);
             photo_picture_free(picture, 1, 1);
          }
        pl->thumb.nb = 0;
     }

   if (pl->thumb.popup)
     {
        photo_popup_warn_del(pl->thumb.popup);
        pl->thumb.popup = NULL;
     }
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
