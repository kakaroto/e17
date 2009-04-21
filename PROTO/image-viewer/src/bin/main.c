/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2,t0,(0
 */
#include <Eet.h>
#include <Ecore.h>
#include <Ecore_File.h>
#include <Elementary.h>

#include "config.h"

#define CONFIG_VERSION 1

#ifdef HAVE_ETHUMB
  #include <Ethumb.h>
#endif

#ifdef HAVE_LIBEXIF
  #include <exif-data.h>
#endif

#define DBG(...) EINA_ERROR_PDBG(__VA_ARGS__)
#define ERR(...) EINA_ERROR_PERR(__VA_ARGS__)

typedef enum _IV_Image_Fit {
     PAN,
     FIT,
     FIT_SCALE,
     ZOOM
} IV_Image_Fit;

typedef enum _IV_Image_Dest {
     IMAGE_CURRENT,
     IMAGE_NEXT,
     IMAGE_PREV
} IV_Image_Dest;

typedef enum _IV_Image_Bg {
     IMAGE_BG_BLACK,
     IMAGE_BG_CHECKERS
} IV_Image_Bg;

typedef struct _IV IV;
typedef struct _IV_Config IV_Config;
typedef struct _IV_Thumb_Info IV_Thumb_Info;

struct _IV
{
   Eina_List *files, *dirs;
   const char *single_file;
   const char *theme_file;

   Ecore_Idler *idler;

   Ecore_Timer *slideshow_timer;

   struct {
	Evas_Object *win, *ly, *img, *scroller;
	Evas_Object *prev_img, *next_img, *bg;
	Evas_Object *controls, *file_label;
        Evas_Object *next_bt, *prev_bt;
        Evas_Object *hoversel;
        Evas_Object *settings_bt, *slideshow_bt;
	Evas_Object *settings_win;
#ifdef HAVE_ETHUMB
	/* XXX: Replace with carousel when ready */
	Evas_Object *toolbar;
#endif
   } gui;

   struct {
	Eina_Bool next : 1;
	Eina_Bool prev : 1;
	Eina_Bool current : 1;
	Eina_Bool hide_controls : 1;
	Eina_Bool fullscreen : 1;
	Eina_Bool fit_changed : 1;
	Eina_Bool slideshow : 1;
#ifdef HAVE_ETHUMB
	Eina_Bool add_previews : 1;
	Eina_Bool ignore_preview_change : 1;
	Eina_Bool hide_previews : 1;
	Eina_Bool previews_visible : 1;
	Eina_Bool first_preview : 1;
#endif
   } flags;


   IV_Config            *config;
   Eet_Data_Descriptor  *config_edd;
   Ecore_Timer          *config_save;

#ifdef HAVE_ETHUMB
   Eina_List		*thumb_path;
   Eina_List		*preview_files;
   Eina_Hash		*preview_items;
   Ethumb		*ethumb;

   Eina_Bool		 thumb_generator;
#endif
};

struct _IV_Config {
     int          config_version;

     int	  auto_hide_previews;
     double       img_scale;
     double	  slideshow_delay;
     IV_Image_Fit fit;
     IV_Image_Bg  image_bg;
};

struct _IV_Thumb_Info
{
   const char *thumb_path;
   const char *file;
};

/* Prototypes */
static int  on_idler(void *data);
static void slideshow_on(IV *iv);
static void slideshow_off(IV *iv);

static Eina_List *
rewind_list(Eina_List *list)
{
   if (!list) return NULL;
   while (list->prev)
     list = list->prev;

   return list;
}

static void
image_configure(IV *iv)
{
#ifdef HAVE_EXPERIMENTAL
   if (iv->config->fit == ZOOM)
     {
	elm_image_no_scale_set(iv->gui.img, EINA_FALSE);
	elm_image_smooth_set(iv->gui.img, EINA_FALSE);
	elm_image_scale_set(iv->gui.img, EINA_FALSE, EINA_FALSE);
	elm_object_scale_set(iv->gui.img, iv->config->img_scale);
	//evas_object_size_hint_min_set(iv->gui.img, -1, -1);
     }
   else
     {
	elm_image_no_scale_set(iv->gui.img, EINA_TRUE);
	elm_image_smooth_set(iv->gui.img, EINA_TRUE);
	elm_object_scale_set(iv->gui.img, 1.0);

	if (iv->config->fit == PAN)
	  elm_image_scale_set(iv->gui.img, EINA_FALSE, EINA_FALSE);
	else if (iv->config->fit == FIT)
	  elm_image_scale_set(iv->gui.img, EINA_FALSE, EINA_TRUE);
	else
	  elm_image_scale_set(iv->gui.img, EINA_TRUE, EINA_TRUE);
     }
#else
   if (iv->config->fit == ZOOM)
     {
	elm_icon_no_scale_set(iv->gui.img, EINA_FALSE);
	elm_icon_smooth_set(iv->gui.img, EINA_FALSE);
	elm_icon_scale_set(iv->gui.img, EINA_FALSE, EINA_FALSE);
	elm_object_scale_set(iv->gui.img, iv->config->img_scale);
	//evas_object_size_hint_min_set(iv->gui.img, -1, -1);
     }
   else
     {
	elm_icon_no_scale_set(iv->gui.img, EINA_TRUE);
	elm_icon_smooth_set(iv->gui.img, EINA_TRUE);
	elm_object_scale_set(iv->gui.img, 1.0);

	if (iv->config->fit == PAN)
	  elm_icon_scale_set(iv->gui.img, EINA_FALSE, EINA_FALSE);
	else if (iv->config->fit == FIT)
	  elm_icon_scale_set(iv->gui.img, EINA_FALSE, EINA_TRUE);
	else
	  elm_icon_scale_set(iv->gui.img, EINA_TRUE, EINA_TRUE);
     }
#endif
}

static void
set_image_bg_style(IV *iv)
{
   switch(iv->config->image_bg)
     {
      case IMAGE_BG_BLACK:
	 elm_bg_file_set(iv->gui.bg, iv->theme_file, "iv/main/bg/black");
	 break;
      case IMAGE_BG_CHECKERS:
	 elm_bg_file_set(iv->gui.bg, iv->theme_file, "iv/main/bg/checkers");
	 break;
     }
}

static int
on_config_save_tick(void *data)
{
   IV *iv = data;
   Eet_File *ef;
   char buf[4096], buf2[4096];
   int ret;

   snprintf(buf, sizeof(buf), "%s/.config/image-viewer/image-viewer.cfg", getenv("HOME"));
   snprintf(buf2, sizeof(buf2), "%s.tmp", buf);

   ef = eet_open(buf2, EET_FILE_MODE_WRITE);
   if (ef)
     {
        eet_data_write(ef, iv->config_edd, "config", iv->config, 1);

        if (eet_close(ef))
          goto save_end;

        ret = ecore_file_mv(buf2, buf);
        if (!ret)
          goto save_end;

        ecore_file_unlink(buf2);
     }

save_end:
   if (iv->config_save)
     ecore_timer_del(iv->config_save);
   iv->config_save = NULL;
   return ECORE_CALLBACK_CANCEL;
}

static void
config_save(IV *iv)
{
   if (iv->config_save)
     ecore_timer_del(iv->config_save);
   iv->config_save = ecore_timer_add(30, on_config_save_tick, iv);
}

static void
config_free(IV *iv)
{
   free(iv->config);
   iv->config = NULL;
}

static int
config_load(IV *iv)
{
   Eet_File *ef;
   char buf[4096], buf2[4096];

   snprintf(buf2, sizeof(buf2), "%s/.config/image-viewer", getenv("HOME"));
   ecore_file_mkpath(buf2);
   snprintf(buf, sizeof(buf), "%s/image-viewer.cfg", buf2);

   ef = eet_open(buf, EET_FILE_MODE_READ);
   if (!ef)
     {
        config_free(iv);
        iv->config = calloc(1, sizeof(IV_Config));
        return 0;
     }

   iv->config = eet_data_read(ef, iv->config_edd, "config");
   eet_close(ef);

   if (iv->config->config_version > CONFIG_VERSION)
     {
        config_free(iv);
        iv->config = calloc(1, sizeof(IV_Config));
        return 0;
     }

   if (iv->config->config_version < CONFIG_VERSION)
     return -1;

   return 1;
}

static void
iv_exit(IV *iv)
{
   on_config_save_tick(iv);
   elm_exit();
}

// generic callback - delete any window (close button/remove) and it just exits
static void
on_win_del_req(void *data, Evas_Object *obj, void *event_info)
{
   /* called when my_win_main is requested to be deleted */
   iv_exit(data); /* exit the program's main loop that runs in elm_run() */
}

static void
on_controls_click(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   IV *iv = data;

   iv->flags.hide_controls = EINA_TRUE;
   if (!iv->idler)
     iv->idler = ecore_idler_add(on_idler, iv);
}

static void
on_image_click(void *data, Evas_Object *obj, void *event_info)
{
   IV *iv = data;

   if (iv->config->fit == PAN || iv->config->fit == ZOOM)
     return;

   iv->flags.next = EINA_TRUE;
   iv->flags.hide_controls = EINA_TRUE;
   if (!iv->idler)
     iv->idler = ecore_idler_add(on_idler, iv);
}

static void
on_settings_slideshow_delay_change(void *data, Evas_Object *obj, void *event_info)
{
   IV *iv = data;

   iv->config->slideshow_delay = elm_slider_value_get(obj);
   config_save(iv);
   if (iv->flags.slideshow)
     {
	slideshow_off(iv);
	slideshow_on(iv);
     }
}

static void
on_settings_bg_toggle_change(void *data, Evas_Object *obj, void *event_info)
{
   IV *iv = data;

   if (elm_toggle_state_get(obj))
     iv->config->image_bg = IMAGE_BG_CHECKERS;
   else
     iv->config->image_bg = IMAGE_BG_BLACK;

   set_image_bg_style(iv);
   config_save(iv);
}

static void
on_settings_auto_hide_previews_toggle_change(void *data, Evas_Object *obj, void *event_info)
{
   IV *iv = data;

   iv->config->auto_hide_previews = elm_toggle_state_get(obj);

   config_save(iv);
}

static void
on_settings_close_click(void *data, Evas_Object *obj, void *event_info)
{
   IV *iv = data;

   evas_object_hide(iv->gui.settings_win);
}

static void
zoom_set(IV *iv, Eina_Bool increase)
{
   if (iv->config->fit != ZOOM)
     {
	int iw, w;
	double scale = elm_object_scale_get(iv->gui.img);

	evas_object_size_hint_max_get(iv->gui.img, &iw, NULL);
	evas_object_geometry_get(iv->gui.img, NULL, NULL, &w, NULL);

	if (w > iw)
	  iv->config->img_scale = scale;
	else
	  {
	     if (scale == 1.0)
	       iv->config->img_scale = (double) w / (double) iw;
	     else
	       iv->config->img_scale = scale;
	  }
     }

   if (iv->config->img_scale >= 1)
     {
	if (increase)
	  iv->config->img_scale += 0.5;
	else
	  iv->config->img_scale -= 0.5;
     }
   else
     {
	if (increase)
	  iv->config->img_scale += 0.05;
	else
	  iv->config->img_scale -= 0.05;
     }

   if (iv->config->img_scale > 0.96 && iv->config->img_scale < 1.5)
     iv->config->img_scale = 1.0;
   else if (iv->config->img_scale < 0.05)
     iv->config->img_scale = 0.05;

   iv->config->fit = ZOOM;
   iv->flags.fit_changed = EINA_TRUE;
   config_save(iv);
   if (!iv->idler)
     iv->idler = ecore_idler_add(on_idler, iv);
}

static void
fullscreen(IV *iv)
{
   iv->flags.fullscreen = EINA_TRUE;
   elm_win_fullscreen_set(iv->gui.win, 1);
}

static void
unfullscreen(IV *iv)
{
   iv->flags.fullscreen = EINA_FALSE;
   elm_win_fullscreen_set(iv->gui.win, 0);
}

static void
read_image(IV *iv, IV_Image_Dest dest)
{
   Evas_Object *img;
   Eina_List *l;

   switch (dest)
     {
      case IMAGE_CURRENT:
	 l = iv->files;
	 break;
      case IMAGE_NEXT:
	 l = iv->files->next;

	 if (!l)
	   {
	      l = rewind_list(l);
	      if (l == iv->files)
		l = NULL;
	   }
	 break;
      case IMAGE_PREV:
	 l = iv->files->prev;

	 if (!l)
	   {
	      l = eina_list_last(iv->files);
	      if (l == iv->files)
		l = NULL;
	   }
	 break;
     }

   while (l)
     {
	Eina_Bool succ = EINA_FALSE;

#ifdef HAVE_EXPERIMENTAL
	img = elm_image_add(iv->gui.ly);
	succ = elm_image_file_set(img, l->data, NULL);
#else
	img = elm_icon_add(iv->gui.ly);
	succ = elm_icon_file_set(img, l->data, NULL);
	elm_icon_prescale_set(img, 0);
#endif
	if (succ)
	  {
	     int orientation = 0;
#ifdef HAVE_LIBEXIF
	     int value = 0;
	     ExifData  *exif = exif_data_new_from_file(l->data);
	     ExifEntry *entry = NULL;
	     ExifByteOrder bo;
	    
	     if (exif)
	       {
		  entry = exif_data_get_entry(exif, EXIF_TAG_ORIENTATION);
		  if (entry)
		    {
		       bo = exif_data_get_byte_order(exif);
		       orientation = exif_get_short(entry->data, bo);
		    }
		  exif_data_free(exif);
	       }
#endif
#ifdef HAVE_EXPERIMENTAL
	     if (orientation > 1 && orientation < 9)
	       {
		  Elm_Image_Orient t1 = ELM_IMAGE_ORIENT_NONE;

		  switch (orientation)
		    {
		     case 2:		/* Horizontal flip */
			t1 = ELM_IMAGE_FLIP_HORIZONTAL;
			break;
		     case 3:		/* Rotate 180 clockwise */
			t1 = ELM_IMAGE_ROTATE_180_CW;
			break;
		     case 4:		/* Vertical flip */
			t1 = ELM_IMAGE_FLIP_VERTICAL;
			break;
		     case 5:		/* Transpose */
			t1 = ELM_IMAGE_FLIP_TRANSPOSE;
			break;
		     case 6:		/* Rotate 90 clockwise */
			t1 = ELM_IMAGE_ROTATE_90_CW;
			break;
		     case 7:		/* Transverse */
			t1 = ELM_IMAGE_FLIP_TRANSVERSE;
			break;
		     case 8:		/* Rotate 90 counter-clockwise */
			t1 = ELM_IMAGE_ROTATE_90_CCW;
			break;
		    }
		  if (t1)
		    elm_image_orient_set(img, t1);
	       }
#endif

	     switch (dest)
	       {
		case IMAGE_CURRENT:
		   iv->gui.img = img;
		   image_configure(iv);
		   edje_object_part_text_set(iv->gui.file_label,
					     "iv.text.label",
					     (char *) ecore_file_file_get(iv->files->data));
		   elm_scroller_content_set(iv->gui.scroller, img);
		   evas_object_show(img);
		   break;
		case IMAGE_NEXT:
		   iv->gui.next_img = img;
		   break;
		case IMAGE_PREV:
		   iv->gui.prev_img = img;
		   break;
	       }
	     evas_object_smart_callback_add(img, "clicked", on_image_click, iv);
	     break;
	  }
	else
	  {
	     eina_stringshare_del(l->data);
	     iv->files = eina_list_remove_list(iv->files, l);
	     evas_object_del(img);
	     switch (dest)
	       {
		case IMAGE_CURRENT:
		   l = iv->files;
		   break;
		case IMAGE_NEXT:
		   l = iv->files->next;
		   break;
		case IMAGE_PREV:
		   l = iv->files->prev;
		   break;
	       }
	     continue;
	  }
     }
}

static void
on_prev_click(void *data, Evas_Object *obj, void *event_info)
{
   IV *iv = data;
   iv->flags.prev = EINA_TRUE;
   if (!iv->idler)
     iv->idler = ecore_idler_add(on_idler, iv);
}

static void
on_next_click(void *data, Evas_Object *obj, void *event_info)
{
   IV *iv = data;
   iv->flags.next = EINA_TRUE;
   if (!iv->idler)
     iv->idler = ecore_idler_add(on_idler, iv);
}

static void
on_toolbar_change(void *data, Evas_Object *obj, void *event_info)
{
#ifdef HAVE_ETHUMB
   Evas_Object *thumb = data;
   IV *iv = evas_object_data_get(thumb, "iv");
   const char *file = evas_object_data_get(thumb, "iv_file");

   if (iv->flags.ignore_preview_change)
     {
	iv->flags.ignore_preview_change = EINA_FALSE;
	return;
     }

   iv->files = eina_list_data_find_list(rewind_list(iv->files), file);
   iv->flags.current = EINA_TRUE;
   if (iv->config->auto_hide_previews)
     iv->flags.hide_previews = EINA_TRUE;
   if (!iv->idler)
     iv->idler = ecore_idler_add(on_idler, iv);
#endif
}

static void *
on_thumb_thread_create(void *data)
{
#ifdef HAVE_ETHUMB
   IV *iv = data;
   Eina_List *l;
   const char *file;
   int count = 0;

   EINA_LIST_FOREACH(iv->preview_files, l, file)
     {
	Ethumb_File *ef = ethumb_file_new(iv->ethumb, file);

	if (ef)
	  {
	     if (ethumb_file_generate(ef))
	       {
		  IV_Thumb_Info *info = calloc(1, sizeof(IV_Thumb_Info));

		  info->thumb_path = eina_stringshare_add(ef->thumb_path);
		  info->file = file;

		  iv->thumb_path = eina_list_append(iv->thumb_path, info);
	       }
	     ethumb_file_free(ef);
	  }

	iv->preview_files = l;

	if (++count > 10)
	  {
	     if (l->next)
	       iv->preview_files = l->next;
	     break;
	  }
     }

   if (iv->preview_files->next)
     {
	iv->flags.add_previews = EINA_TRUE;
	if (!iv->idler)
	  iv->idler = ecore_idler_add(on_idler, iv);
     }
   else
     iv->flags.add_previews = EINA_FALSE;

   iv->thumb_generator = EINA_FALSE;
   return NULL;
#endif
}

static int
on_idler(void *data)
{
   IV *iv = data;
   Eina_Bool renew = EINA_FALSE;
#ifdef HAVE_ETHUMB
   Elm_Toolbar_Item *item;
#endif

   if (iv->dirs)
     {
	Eina_List *files;
	char *dir, *file, buf[4096];

	dir = iv->dirs->data;

	iv->dirs = eina_list_remove_list(iv->dirs, iv->dirs);
	files = ecore_file_ls(dir);
	EINA_LIST_FREE(files, file)
	  {
	     snprintf(buf, sizeof(buf), "%s/%s", dir, file);
	     free(file);
	     /* XXX: recursive scanning with an option */
	     if (!ecore_file_is_dir(buf))
	       iv->files = eina_list_append(iv->files,
					    eina_stringshare_add(buf));
	  }

	eina_stringshare_del(dir);

	if (iv->single_file)
	  {
	     iv->files = eina_list_data_find_list(iv->files, iv->single_file);
	     eina_stringshare_del(iv->single_file);
	     iv->single_file = NULL;
	  }

	if (!renew && iv->dirs)
	  renew = EINA_TRUE;
#ifdef HAVE_ETHUMB
	iv->flags.add_previews = EINA_TRUE;
#endif
     }

   /* Display the first image */
   if (!iv->gui.img)
     read_image(iv, IMAGE_CURRENT);
   else
     {
	Evas_Object *img;

	if (!iv->gui.prev_img)
	  {
	     read_image(iv, IMAGE_PREV);

	     if (iv->gui.prev_img)
	       evas_object_show(iv->gui.prev_bt);
	     else
	       evas_object_hide(iv->gui.prev_bt);
	  }
	if (!iv->gui.next_img)
	  {
	     read_image(iv, IMAGE_NEXT);

	     if (iv->gui.next_img)
	       evas_object_show(iv->gui.next_bt);
	     else
	       evas_object_hide(iv->gui.next_bt);
	  }

#ifdef HAVE_ETHUMB
	if (!iv->thumb_generator && iv->thumb_path)
	  {
	     Evas_Object *thumb;
	     IV_Thumb_Info *info;

	     EINA_LIST_FREE(iv->thumb_path, info)
	       {
		  Elm_Toolbar_Item *item;

		  thumb = elm_icon_add(iv->gui.toolbar);
		  elm_icon_file_set(thumb, info->thumb_path, NULL);
		  evas_object_data_set(thumb, "iv", iv);
		  evas_object_data_set(thumb, "iv_file", info->file);
		  evas_object_show(thumb);
		  item = elm_toolbar_item_add(iv->gui.toolbar, thumb, NULL, on_toolbar_change, thumb);
		  eina_hash_add(iv->preview_items, info->file, item);

		  if (iv->flags.first_preview)
		    {
		       iv->flags.ignore_preview_change = EINA_TRUE;
		       elm_toolbar_item_select(item);
		       iv->flags.first_preview = EINA_FALSE;
		    }

		  eina_stringshare_del(info->thumb_path);
		  free(info);
	       }

	  }

	if (iv->flags.add_previews)
	  {
	     int rc;

	     if (!iv->preview_files)
	       {
		  iv->preview_files = rewind_list(iv->files);
		  iv->flags.first_preview = EINA_TRUE;
	       }

	     if (!iv->thumb_generator)
	       {
		  iv->thumb_generator = EINA_TRUE;
		  on_thumb_thread_create(iv);
	       }
	  }
#endif
     }

   if (!renew && iv->files && 
       (!iv->gui.img || !iv->gui.prev_img || !iv->gui.next_img))
     renew = EINA_TRUE;

   if (iv->flags.next)
     {
	if (iv->gui.next_img)
	  {
	     iv->flags.next = EINA_FALSE;

	     if (iv->files->next)
	       iv->files = iv->files->next;
	     else
	       iv->files = rewind_list(iv->files);

	     /* XXX: this is necessary for some reason, bug in elm? */
	     elm_scroller_content_set(iv->gui.scroller, NULL);

	     if (iv->gui.prev_img)
	       {
		  evas_object_smart_callback_del(iv->gui.prev_img, "clicked", on_image_click);
		  evas_object_del(iv->gui.prev_img);
	       }
	     iv->gui.prev_img = iv->gui.img;
	     evas_object_hide(iv->gui.img);

	     iv->gui.img = iv->gui.next_img;
	     image_configure(iv);
	     edje_object_part_text_set(iv->gui.file_label,
				       "iv.text.label",
				       (char *) ecore_file_file_get(iv->files->data));
	     elm_scroller_content_set(iv->gui.scroller, iv->gui.img);
	     evas_object_show(iv->gui.img);
	     iv->gui.next_img = NULL;

#ifdef HAVE_ETHUMB
	     iv->flags.ignore_preview_change = EINA_TRUE;
	     item = eina_hash_find(iv->preview_items, iv->files->data);
	     if (item)
	       elm_toolbar_item_select(item);
#endif
	  }
     }
   else if (iv->flags.prev)
     {
	if (iv->gui.prev_img)
	  {
	     iv->flags.prev = EINA_FALSE;

	     if (iv->files->prev)
	       iv->files = iv->files->prev;
	     else
	       iv->files = eina_list_last(iv->files);

	     /* XXX: this is necessary for some reason, bug in elm? */
	     elm_scroller_content_set(iv->gui.scroller, NULL);

	     if (iv->gui.next_img)
	       {
		  evas_object_smart_callback_del(iv->gui.next_img, "clicked", on_image_click);
		  evas_object_del(iv->gui.next_img);
	       }
	     iv->gui.next_img = iv->gui.img;
	     evas_object_hide(iv->gui.img);

	     iv->gui.img = iv->gui.prev_img;
	     image_configure(iv);
	     edje_object_part_text_set(iv->gui.file_label,
				       "iv.text.label",
				       (char *) ecore_file_file_get(iv->files->data));
	     elm_scroller_content_set(iv->gui.scroller, iv->gui.img);
	     evas_object_show(iv->gui.img);
	     iv->gui.prev_img = NULL;

#ifdef HAVE_ETHUMB
	     iv->flags.ignore_preview_change = EINA_TRUE;
	     item = eina_hash_find(iv->preview_items, iv->files->data);
	     if (item)
	       elm_toolbar_item_select(item);
#endif
	  }
     }
   else if (iv->flags.current)
     {
	iv->flags.current = EINA_FALSE;

	if (iv->gui.prev_img)
	  {
	     evas_object_smart_callback_del(iv->gui.prev_img, "clicked", on_image_click);
	     evas_object_del(iv->gui.prev_img);
	     iv->gui.prev_img = NULL;
	  }
	if (iv->gui.next_img)
	  {
	     evas_object_smart_callback_del(iv->gui.next_img, "clicked", on_image_click);
	     evas_object_del(iv->gui.next_img);
	     iv->gui.next_img = NULL;
	  }

	if (iv->gui.img)
	  {
	     evas_object_smart_callback_del(iv->gui.img, "clicked", on_image_click);
	     evas_object_del(iv->gui.img);
	     iv->gui.img = NULL;
	  }

	elm_scroller_content_set(iv->gui.scroller, NULL);
	read_image(iv, IMAGE_CURRENT);
     }

   if (iv->flags.hide_controls)
     {
	iv->flags.hide_controls = EINA_FALSE;

	edje_object_signal_emit(elm_layout_edje_get(iv->gui.ly),
			       	"iv,state,hide_controls", "iv");
	elm_hoversel_hover_end(iv->gui.hoversel);
     }
#ifdef HAVE_ETHUMB
   else if (iv->flags.hide_previews)
     {
	iv->flags.hide_previews = EINA_FALSE;

	edje_object_signal_emit(elm_layout_edje_get(iv->gui.ly),
			       	"iv,state,hide_previews", "iv");
     }
#endif
   if (iv->flags.fit_changed)
     {
	iv->flags.fit_changed = EINA_FALSE;
	switch (iv->config->fit)
	  {
	   case PAN:
	      elm_hoversel_label_set(iv->gui.hoversel, "Pan");
	      break;
	   case FIT:
	      elm_hoversel_label_set(iv->gui.hoversel, "Fit");
	      break;
	   case FIT_SCALE:
	      elm_hoversel_label_set(iv->gui.hoversel, "Fit & Scale");
	      break;
	   default:
	      elm_hoversel_label_set(iv->gui.hoversel, "Fitting");
	      break;
	  }
	if (iv->gui.img)
	  image_configure(iv);
     }

   if (renew)
     return ECORE_CALLBACK_RENEW;
   else
     {
	iv->idler = NULL;
	return ECORE_CALLBACK_CANCEL;
     }
}

static int
on_slideshow_tick(void *data)
{
   IV *iv = data;

   iv->flags.next = EINA_TRUE;
   iv->flags.hide_controls = EINA_TRUE;
   on_idler(data);

   return ECORE_CALLBACK_RENEW;
}

static void
slideshow_on(IV *iv)
{
   edje_object_signal_emit(elm_layout_edje_get(iv->gui.controls),
			   "iv,state,slideshow_off", "iv");
   iv->slideshow_timer = ecore_timer_add(iv->config->slideshow_delay, on_slideshow_tick, iv);
   iv->flags.slideshow = EINA_TRUE;
}

static void
slideshow_off(IV *iv)
{
   edje_object_signal_emit(elm_layout_edje_get(iv->gui.controls),
			   "iv,state,slideshow_on", "iv");

   if (iv->slideshow_timer)
     ecore_timer_del(iv->slideshow_timer);
   iv->slideshow_timer = NULL;
   iv->flags.slideshow = EINA_FALSE;
}

static void
on_slideshow_click(void *data, Evas_Object *obj, void *event_info)
{
   IV *iv = data;
   if (!iv->flags.slideshow)
     slideshow_on(iv);
   else
     slideshow_off(iv);
}

static void
on_settings_click(void *data, Evas_Object *obj, void *event_info)
{
   IV *iv = data;

   if (!iv->gui.settings_win)
     {
	Evas_Object *o, *bx, *bx2, *ic;

	iv->gui.settings_win = o = elm_win_inwin_add(iv->gui.win);
	elm_win_inwin_style_set(o, "shadow");
	o = elm_bg_add(iv->gui.settings_win);
	evas_object_size_hint_weight_set(o, 1.0, 1.0);

	bx = elm_box_add(iv->gui.settings_win);
	o = elm_slider_add(bx);
	elm_slider_label_set(o, "Slideshow: image delay");
	elm_slider_span_size_set(o, 120);
	evas_object_size_hint_align_set(o, -1.0, 0.5);
	evas_object_size_hint_weight_set(o, 1.0, 1.0);
	elm_slider_indicator_format_set(o, "%.2f");
	elm_slider_min_max_set(o, 1.0, 10.0);
	elm_slider_unit_format_set(o, "%1.1f seconds");
	elm_slider_value_set(o, iv->config->slideshow_delay);
	evas_object_smart_callback_add(o, "delay,changed",
				       on_settings_slideshow_delay_change, iv);
	evas_object_show(o);
	elm_box_pack_end(bx, o);

	o = elm_toggle_add(bx);
	elm_toggle_label_set(o, "Image background");
	elm_toggle_states_labels_set(o, "checkers", "black");
        if (iv->config->image_bg == IMAGE_BG_BLACK)
          elm_toggle_state_set(o, 0);
        else
          elm_toggle_state_set(o, 1);
	elm_box_pack_end(bx, o);
	evas_object_smart_callback_add(o, "changed",
				       on_settings_bg_toggle_change, iv);
	evas_object_show(o);

#ifdef HAVE_ETHUMB
	o = elm_toggle_add(bx);
	elm_toggle_label_set(o, "Auto-hide the preview pane");
	elm_toggle_state_set(o, iv->config->auto_hide_previews);
	elm_box_pack_end(bx, o);
	evas_object_smart_callback_add(o, "changed",
				       on_settings_auto_hide_previews_toggle_change, iv);
	evas_object_show(o);
#endif

	bx2 = elm_box_add(bx);
	elm_box_horizontal_set(bx2, EINA_TRUE);
	evas_object_show(bx2);
	elm_box_pack_end(bx, bx2);

	ic = elm_icon_add(bx2);
	elm_icon_standard_set(ic, "close");
	o = elm_button_add(bx2);
	elm_icon_scale_set(ic, 0, 0);
	elm_button_icon_set(o, ic);
	elm_button_label_set(o, "Close");
	evas_object_smart_callback_add(o, "clicked",
				       on_settings_close_click, iv);
	evas_object_show(o);
	elm_box_pack_end(bx2, o);

	elm_win_inwin_content_set(iv->gui.settings_win, bx);
     }

   if (evas_object_visible_get(iv->gui.settings_win))
     evas_object_hide(iv->gui.settings_win);
   else
     elm_win_inwin_activate(iv->gui.settings_win);
}

static void
on_hoversel_fs(void *data, Evas_Object *obj, void *event_info)
{
   IV *iv = data;

   if (!iv) return;
   iv->flags.fit_changed = EINA_TRUE;
   iv->config->fit = FIT_SCALE;
   config_save(iv);
   if (!iv->idler)
     iv->idler = ecore_idler_add(on_idler, iv);
}

static void
on_hoversel_f(void *data, Evas_Object *obj, void *event_info)
{
   IV *iv = data;

   if (!iv) return;
   iv->flags.fit_changed = EINA_TRUE;
   iv->config->fit = FIT;
   config_save(iv);
   if (!iv->idler)
     iv->idler = ecore_idler_add(on_idler, iv);
}

static void
on_hoversel_p(void *data, Evas_Object *obj, void *event_info)
{
   IV *iv = data;

   if (!iv) return;
   iv->flags.fit_changed = EINA_TRUE;
   iv->config->fit = PAN;
   config_save(iv);
   if (!iv->idler)
     iv->idler = ecore_idler_add(on_idler, iv);
}

static void
on_key_down(void *data, Evas *a, Evas_Object *obj, void *event_info)
{
   IV *iv = data;
   Evas_Event_Key_Down *ev = event_info;

   if (!iv || !ev) return;
   if (!strcmp(ev->keyname, "space") || !strcmp(ev->keyname, "Right"))
     iv->flags.next = EINA_TRUE;
   else if (!strcmp(ev->keyname, "Left"))
     iv->flags.prev = EINA_TRUE;
   else if (!strcmp(ev->keyname, "q") || ((!strcmp(ev->keyname, "Escape") && !iv->flags.fullscreen && !iv->flags.slideshow)))
     iv_exit(iv);
   else if (!strcmp(ev->keyname, "equal"))
     zoom_set(iv, EINA_TRUE);
   else if (!strcmp(ev->keyname, "minus"))
     zoom_set(iv, EINA_FALSE);
   else if (!strcmp(ev->keyname, "1"))
     {
	iv->config->fit = PAN;
	iv->flags.fit_changed = EINA_TRUE;
	config_save(iv);
     }
   else if (!strcmp(ev->keyname, "2"))
     {
	iv->config->fit = FIT;
	iv->flags.fit_changed = EINA_TRUE;
	config_save(iv);
     }
   else if (!strcmp(ev->keyname, "3"))
     {
	iv->config->fit = FIT_SCALE;
	iv->flags.fit_changed = EINA_TRUE;
	config_save(iv);
     }

   if (iv->flags.fullscreen)
     {
	if (!strcmp(ev->keyname, "F11") || !strcmp(ev->keyname, "Escape") || !strcmp(ev->keyname, "f"))
	  unfullscreen(iv);
     }
   else
     {
	if (!strcmp(ev->keyname, "F11") || !strcmp(ev->keyname, "f"))
	  fullscreen(iv);
     }

#ifdef HAVE_ETHUMB
   if (!strcmp(ev->keyname, "F9"))
     edje_object_signal_emit(elm_layout_edje_get(iv->gui.ly),
			     "iv,state,toggle_previews", "iv");
#endif

   if (iv->flags.slideshow)
     {
	if (!strcmp(ev->keyname, "F5") || !strcmp(ev->keyname, "Escape"))
	  slideshow_off(iv);
     }
   else
     {
	if (!strcmp(ev->keyname, "F5"))
	  slideshow_on(iv);
     }

   if (!iv->idler)
     iv->idler = ecore_idler_add(on_idler, iv);
}

// main window - setup
static void
create_main_win(IV *iv)
{
   Evas_Object *o, *bx, *ic;
   char buf[4096];
   
   snprintf(buf, sizeof(buf), "%s/themes/default.edj", PACKAGE_DATA_DIR);
   elm_theme_extension_add(buf);
   iv->theme_file = eina_stringshare_add(buf);

   o = elm_win_add(NULL, "main", ELM_WIN_BASIC);
   elm_win_title_set(o, "Image Viewer");
   evas_object_smart_callback_add(o, "delete-request", on_win_del_req, iv);
   evas_object_resize(o, 200, 180);
   iv->gui.win = o;

   o = elm_bg_add(iv->gui.win);
   elm_win_resize_object_add(iv->gui.win, o);
   evas_object_size_hint_weight_set(o, 1.0, 1.0);
   evas_object_show(o);
   iv->gui.bg = o;
   set_image_bg_style(iv);
   
   /*
    * XXX: configurable themes
    * */
   o = elm_layout_add(iv->gui.win);
   elm_layout_file_set(o, buf, "iv/main");
   evas_object_size_hint_weight_set(o, 1.0, 1.0);
   elm_win_resize_object_add(iv->gui.win, o);
   evas_object_focus_set(o, 1);
   evas_object_event_callback_add(o, EVAS_CALLBACK_KEY_DOWN, on_key_down, iv);
   evas_object_show(o);
   iv->gui.ly = o;

   o = elm_scroller_add(iv->gui.ly);
   evas_object_size_hint_weight_set(o, 1.0, 1.0);
   elm_win_resize_object_add(iv->gui.win, o);
   elm_layout_content_set(iv->gui.ly, "iv.swallow.image", o);
   iv->gui.scroller = o;

#ifdef HAVE_ETHUMB
   o = elm_toolbar_add(iv->gui.ly);
   evas_object_size_hint_weight_set(o, 0.0, 0.0);
   evas_object_size_hint_align_set(o, -1.0, 0.0);
   elm_layout_content_set(iv->gui.ly, "iv.swallow.previews", o);
   evas_object_show(o);
   iv->gui.toolbar = o;
#endif

   o = elm_layout_add(iv->gui.ly);
   elm_layout_file_set(o, buf, "iv/controls");
   evas_object_size_hint_weight_set(o, 1.0, 1.0);
   elm_layout_content_set(iv->gui.ly, "iv.swallow.controls", o);
   edje_object_signal_callback_add(elm_layout_edje_get(o),
				   "iv,action,click", "", on_controls_click, iv);
   evas_object_show(o);
   iv->gui.controls = o;

   ic = elm_icon_add(iv->gui.controls);
   elm_icon_file_set(ic, buf, "iv/controls/prev");
   elm_icon_scale_set(ic, 0, 0);
   evas_object_size_hint_align_set(ic, 0.5, 0.5);
   evas_object_show(ic);
   
   o = elm_button_add(iv->gui.controls);
   elm_button_icon_set(o, ic);
   evas_object_size_hint_align_set(o, 0.5, 0.5);
   elm_layout_content_set(iv->gui.controls, "iv.swallow.prev", o);
   evas_object_smart_callback_add(o, "clicked", on_prev_click, iv);
   evas_object_show(o);
   iv->gui.prev_bt = o;

   ic = elm_icon_add(iv->gui.controls);
   elm_icon_file_set(ic, buf, "iv/controls/next");
   elm_icon_scale_set(ic, 0, 0);
   evas_object_size_hint_align_set(ic, 0.5, 0.5);
   evas_object_show(ic);
   
   o = elm_button_add(iv->gui.controls);
   elm_button_icon_set(o, ic);
   evas_object_size_hint_align_set(o, 0.5, 0.5);
   elm_layout_content_set(iv->gui.controls, "iv.swallow.next", o);
   evas_object_smart_callback_add(o, "clicked", on_next_click, iv);
   evas_object_show(o);
   iv->gui.next_bt = o;

   ic = elm_icon_add(iv->gui.controls);
   elm_icon_file_set(ic, buf, "iv/controls/slideshow");
   elm_icon_scale_set(ic, 0, 0);
   evas_object_size_hint_align_set(ic, 0.5, 0.5);
   evas_object_show(ic);
   
   o = elm_button_add(iv->gui.controls);
   elm_button_icon_set(o, ic);
   evas_object_size_hint_align_set(o, 0.5, 0.5);
   elm_layout_content_set(iv->gui.controls, "iv.swallow.slideshow", o);
   evas_object_smart_callback_add(o, "clicked", on_slideshow_click, iv);
   evas_object_show(o);
   iv->gui.slideshow_bt = o;

   ic = elm_icon_add(iv->gui.controls);
   elm_icon_file_set(ic, buf, "iv/controls/settings");
   elm_icon_scale_set(ic, 0, 0);
   evas_object_size_hint_align_set(ic, 0.5, 0.5);
   evas_object_show(ic);
   
   o = elm_button_add(iv->gui.controls);
   elm_button_icon_set(o, ic);
   evas_object_size_hint_align_set(o, 0.5, 0.5);
   elm_layout_content_set(iv->gui.controls, "iv.swallow.settings", o);
   evas_object_smart_callback_add(o, "clicked", on_settings_click, iv);
   evas_object_show(o);
   iv->gui.settings_bt = o;

   o = edje_object_add(evas_object_evas_get(iv->gui.controls));
   edje_object_file_set(o, buf, "iv/controls/label");
   elm_layout_content_set(iv->gui.controls, "iv.swallow.label", o);
   evas_object_show(o);
   iv->gui.file_label = o;

   o = elm_hoversel_add(iv->gui.controls);
   elm_layout_content_set(iv->gui.controls, "iv.swallow.hoversel", o);
   elm_hoversel_label_set(o, "Fitting");
   elm_hoversel_item_add(o, "Fit & Scale", NULL, ELM_ICON_NONE, on_hoversel_fs, iv);
   elm_hoversel_item_add(o, "Fit", NULL, ELM_ICON_NONE, on_hoversel_f, iv);
   elm_hoversel_item_add(o, "Pan", NULL, ELM_ICON_NONE, on_hoversel_p, iv);
   elm_hoversel_hover_parent_set(o, iv->gui.win);
   evas_object_size_hint_weight_set(o, 0.0, 0.0);
   evas_object_size_hint_align_set(o, 0.5, 0.5);
   evas_object_show(o);
   iv->gui.hoversel = o;

   evas_object_show(iv->gui.win);
}

static void
iv_free(IV *iv)
{
   const char *file;

   iv->files = rewind_list(iv->files);
   EINA_LIST_FREE(iv->files, file)
      eina_stringshare_del(file);
   EINA_LIST_FREE(iv->dirs, file)
      eina_stringshare_del(file);

   eina_stringshare_del(iv->theme_file);

   if (iv->config_edd)
     eet_data_descriptor_free(iv->config_edd);

   config_free(iv);

   free(iv);
}

static Eina_Hash *
eet_eina_hash_add_alloc(void *h, const void *key, void *data)
{
   Eina_Hash *hash = h;
   if (!hash) hash = eina_hash_string_superfast_new(NULL);
   if (!hash) return NULL;
   eina_hash_add(hash, key, data);
   return hash;
}

static void
config_init(IV *iv)
{
   Eet_Data_Descriptor_Class eddc;
   
   eddc.version = EET_DATA_DESCRIPTOR_CLASS_VERSION;
   eddc.func.mem_alloc = NULL;
   eddc.func.mem_free = NULL;
   eddc.func.str_alloc = (char *(*)(const char *)) eina_stringshare_add;
   eddc.func.str_free = (void (*)(const char *)) eina_stringshare_del;
   eddc.func.list_next = (void *(*)(void *)) eina_list_next;
   eddc.func.list_append = (void *(*)(void *l, void *d)) eina_list_append;
   eddc.func.list_data = (void *(*)(void *)) eina_list_data_get;
   eddc.func.list_free = (void *(*)(void *)) eina_list_free;
   eddc.func.hash_foreach =
      (void  (*) (void *, int (*) (void *, const char *, void *, void *), void *))
      eina_hash_foreach;
   eddc.func.hash_add = (void* (*) (void *, const char *, void *)) eet_eina_hash_add_alloc;
   eddc.func.hash_free = (void  (*) (void *)) eina_hash_free;
   eddc.name = "IV_Config";
   eddc.size = sizeof(IV_Config);

   iv->config_edd = eet_data_descriptor2_new(&eddc);
#undef T
#undef D
#define T IV_Config 
#define D iv->config_edd
#define C_VAL(edd, type, member, dtype) EET_DATA_DESCRIPTOR_ADD_BASIC(edd, type, #member, member, dtype)
   C_VAL(D, T, config_version, EET_T_INT);
   C_VAL(D, T, img_scale, EET_T_DOUBLE);
   C_VAL(D, T, slideshow_delay, EET_T_DOUBLE);
   C_VAL(D, T, fit, EET_T_INT);
   C_VAL(D, T, image_bg, EET_T_INT);

   switch (config_load(iv))
     {
      case 0:
         iv->config->fit = FIT;
         iv->config->img_scale = 1.0;
         iv->config->slideshow_delay = 4.0;
         iv->config->image_bg = IMAGE_BG_BLACK;
         break;
      case -1:
         /* Incremental additions */
         iv->config->config_version = CONFIG_VERSION;
         break;
      default:
         return;
     }
   config_save(iv);
}

EAPI int
elm_main(int argc, char **argv)
{
   int i;
   IV *iv;
   
   iv = calloc(1, sizeof(IV));
   config_init(iv);

   for (i = 1; i < argc; i++)
     {
	if (!strncmp(argv[i], "file://", 7))
	  argv[i] = argv[i] + 7;
	if (!ecore_file_exists(argv[i]))
	  continue;
	if (ecore_file_is_dir(argv[i]))
	  iv->dirs = eina_list_append(iv->dirs,
				      eina_stringshare_add(argv[i]));
	else
	  iv->files = eina_list_append(iv->files,
				       eina_stringshare_add(argv[i]));
     }

   if (!iv->dirs)
     {
	if (!iv->files)
	  iv->dirs = eina_list_append(iv->dirs, eina_stringshare_add("."));
	else if (eina_list_count(iv->files) == 1)
	  {
	     char *dir = ecore_file_dir_get(iv->files->data);

	     iv->dirs = eina_list_append(iv->dirs, eina_stringshare_add(dir));
	     iv->single_file = iv->files->data;
	     iv->files = eina_list_remove_list(iv->files, iv->files);
	     free(dir);
	  }
     }

   create_main_win(iv);

#ifdef HAVE_ETHUMB
   ethumb_init();
   iv->ethumb = ethumb_new();
   iv->preview_items = eina_hash_string_superfast_new(NULL);
   if (iv->files)
     iv->flags.add_previews = EINA_TRUE;
#endif

   iv->flags.fit_changed = EINA_TRUE;
   iv->idler = ecore_idler_add(on_idler, iv);

   elm_run();

#ifdef HAVE_ETHUMB
   ethumb_free(iv->ethumb);
   eina_hash_free(iv->preview_items);
   ethumb_shutdown();
#endif
   iv_free(iv);

   elm_shutdown();
   return 0; 
}

ELM_MAIN()
