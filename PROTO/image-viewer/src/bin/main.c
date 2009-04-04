/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2,t0,(0
 */
#include <Ecore.h>
#include <Ecore_File.h>
#include <Elementary.h>

typedef enum _IV_Image_Fit {
     PAN,
     FIT,
     FIT_SCALE
} IV_Image_Fit;

typedef struct _IV IV;

struct _IV
{
   Eina_List *files, *dirs;
   const char *single_file;

   Ecore_Idle_Enterer *idle_enterer;

   struct {
	Evas_Object *win, *ly, *img, *scroller;
	Evas_Object *prev_img, *next_img;
	Evas_Object *controls, *box;
        Evas_Object *next_bt, *prev_bt;
        Evas_Object *hoversel, *file_label;
   } gui;

   struct {
	Eina_Bool next : 1;
	Eina_Bool prev : 1;
	Eina_Bool hide_controls : 1;
	Eina_Bool fullscreen : 1;
	Eina_Bool fit_changed : 1;
   } flags;

   struct {
	IV_Image_Fit fit;
   } config;

   struct {
	Evas_Coord x, y, w, h, iw, ih;
   } cache;
};

static void
image_configure(IV *iv)
{
   Evas_Coord x, y, w, h, iw, ih;
   Evas_Object *o;

   o = elm_layout_edje_get(iv->gui.ly);
   edje_object_part_geometry_get(o, "iv.swallow.image", &x, &y, &w, &h);
   if (!w || !h) return;

   evas_event_freeze(evas_object_evas_get(iv->gui.img));
   if (!strcmp(evas_object_type_get(iv->gui.img), "edje"))
     {
	if (iv->cache.x != x || iv->cache.y != y ||
	    iv->cache.w != w || iv->cache.h != h)
	  {
	     evas_object_move(iv->gui.img, x, y);
	     evas_object_resize(iv->gui.img, w, h);
	  }
     }
   else
     {
	Evas_Coord ox = x, oy = y, ow = w, oh = h, iw = 0, ih = 0;
	evas_object_image_size_get(iv->gui.img, &iw, &ih);
	
	if (iv->cache.x != x || iv->cache.y != y ||
	    iv->cache.w != w || iv->cache.h != h ||
	    iv->cache.iw != iw || iv->cache.ih != ih)
	  {
	     if (iv->config.fit == PAN)
	       {
		  w = iw;
		  h = ih;
	       }
	     else
	       {
		  h = ((double)ih * w) / (double)iw;
		  if (h > oh)
		    {
		       h = oh;
		       w = ((double)iw * h) / (double)ih;
		    }
		  if (iv->config.fit != FIT_SCALE)
		    {
		       if ((w > iw) || (h > ih))
			 {
			    w = iw;
			    h = ih;
			 }
		    }
	       }
	     x = ox + ((ow - w) / 2);
	     y = oy + ((oh - h) / 2);
	     evas_object_move(iv->gui.img, x, y);
	     evas_object_image_fill_set(iv->gui.img, 0, 0, w, h);
	     evas_object_resize(iv->gui.img, w, h);
	     /* evas_object_size_hint_min_set(iv->gui.img, w, h); */
	  }
	iv->cache.iw = iw;
	iv->cache.ih = ih;

     }
   iv->cache.x = x; iv->cache.y = y;
   iv->cache.w = w; iv->cache.h = h;
   evas_event_thaw(evas_object_evas_get(iv->gui.img));
}

Eina_List *
rewind_list(Eina_List *list)
{
   Eina_List *l = list;

   if (!l)
     return l;
   while (l->prev)
     l = l->prev;
   return l;
}

// generic callback - delete any window (close button/remove) and it just exits
static void
on_win_del_req(void *data, Evas_Object *obj, void *event_info)
{
   /* called when my_win_main is requested to be deleted */
   elm_exit(); /* exit the program's main loop that runs in elm_run() */
}

static void
on_layout_resize(void *data, Evas *a, Evas_Object *obj, void *event_info)
{
   IV *iv = data;

   if (iv->gui.img)
     image_configure(iv);
}

static void
on_image_resize(void *data, Evas *a, Evas_Object *obj, void *event_info)
{
   IV *iv = data;

   if (obj != iv->gui.img)
     return;
   if (iv->gui.img)
     {
	image_configure(iv);
	iv->flags.fit_changed = EINA_TRUE;
     }
}

static void
on_controls_click(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   IV *iv = data;

   iv->flags.hide_controls = EINA_TRUE;
}

static void
on_image_click(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   IV *iv = data;

   iv->flags.next = EINA_TRUE;
   iv->flags.hide_controls = EINA_TRUE;
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
on_key_down(void *data, Evas *a, Evas_Object *obj, void *event_info)
{
   IV *iv = data;
   Evas_Event_Key_Down *ev = event_info;

   if (!iv || !ev) return;
   if (!strcmp(ev->keyname, "space") || !strcmp(ev->keyname, "Right"))
     iv->flags.next = EINA_TRUE;
   else if (!strcmp(ev->keyname, "Left"))
     iv->flags.prev = EINA_TRUE;
   else if (!strcmp(ev->keyname, "q") || (!strcmp(ev->keyname, "Escape") && !iv->flags.fullscreen))
     elm_exit();
   else if (!strcmp(ev->keyname, "1"))
     {
	iv->config.fit = PAN;
	iv->flags.fit_changed = EINA_TRUE;
     }
   else if (!strcmp(ev->keyname, "2"))
     {
	iv->config.fit = FIT;
	iv->flags.fit_changed = EINA_TRUE;
     }
   else if (!strcmp(ev->keyname, "3"))
     {
	iv->config.fit = FIT_SCALE;
	iv->flags.fit_changed = EINA_TRUE;
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
}

static void
on_prev_click(void *data, Evas_Object *obj, void *event_info)
{
   IV *iv = data;
   iv->flags.prev = EINA_TRUE;
}

static void
on_next_click(void *data, Evas_Object *obj, void *event_info)
{
   IV *iv = data;
   iv->flags.next = EINA_TRUE;
}

static int
on_idle_enterer(void *data)
{
   IV *iv = data;

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
	     iv->files = eina_list_append(iv->files, eina_stringshare_add(buf));
	  }

	eina_stringshare_del(dir);
	if (iv->single_file)
	  {
	     iv->files = eina_list_data_find_list(iv->files, iv->single_file);
	     eina_stringshare_del(iv->single_file);
	     iv->single_file = NULL;
	  }
     }

   /* Display the first image */
   if (!iv->gui.img)
     {
	Evas_Object *img;

	while (iv->files)
	  {
	     img = evas_object_image_add(evas_object_evas_get(iv->gui.ly));
	     evas_object_image_file_set(img, iv->files->data, NULL);
	     if (EVAS_LOAD_ERROR_NONE == evas_object_image_load_error_get(img))
	       {
		  iv->gui.img = img;
		  image_configure(iv);
		  elm_label_label_set(iv->gui.file_label,
				      (char *) ecore_file_file_get(iv->files->data));
		  /* elm_scroller_content_set(iv->gui.scroller, img); */
		  elm_layout_content_set(iv->gui.ly, "iv.swallow.image", img);
		  evas_object_show(img);
		  evas_object_event_callback_add(img, EVAS_CALLBACK_RESIZE, on_image_resize, iv);
		  break;
	       }
	     else
	       {
		  iv->files = eina_list_remove_list(iv->files, iv->files);
		  evas_object_del(img);
		  continue;
	       }
	  }
     }
   else
     {
	Evas_Object *img;

	if (!iv->gui.prev_img)
	  {
	     Eina_List *l = iv->files->prev;

	     if (!l)
	       {
		  l = eina_list_last(iv->files);
		  if (l == iv->files)
		    l = NULL;
	       }

	     if (l)
	       evas_object_show(iv->gui.prev_bt);
	     else
	       evas_object_hide(iv->gui.prev_bt);

	     while (l)
	       {
		  img = evas_object_image_add(evas_object_evas_get(iv->gui.ly));
		  evas_object_image_file_set(img, l->data, NULL);
		  if (EVAS_LOAD_ERROR_NONE == evas_object_image_load_error_get(img))
		    {
		       iv->gui.prev_img = img;
		       evas_object_event_callback_add(img, EVAS_CALLBACK_RESIZE, on_image_resize, iv);
		       break;
		    }
		  else
		    {
		       iv->files = eina_list_remove_list(iv->files, l);
		       l = iv->files->prev;
		       evas_object_del(img);
		       continue;
		    }
	       }
	  }
	if (!iv->gui.next_img)
	  {
	     Eina_List *l = iv->files->next;

	     if (!l)
	       {
		  l = rewind_list(iv->files);
		  if (l == iv->files)
		    l = NULL;
	       }

	     if (l)
	       evas_object_show(iv->gui.next_bt);
	     else
	       evas_object_hide(iv->gui.next_bt);

	     while (l)
	       {
		  img = evas_object_image_add(evas_object_evas_get(iv->gui.ly));
		  evas_object_image_file_set(img, l->data, NULL);
		  if (EVAS_LOAD_ERROR_NONE == evas_object_image_load_error_get(img))
		    {
		       evas_object_event_callback_add(img, EVAS_CALLBACK_RESIZE, on_image_resize, iv);
		       iv->gui.next_img = img;
		       break;
		    }
		  else
		    {
		       iv->files = eina_list_remove_list(iv->files, l);
		       l = iv->files->next;
		       evas_object_del(img);
		       continue;
		    }
	       }
	  }
     }

   evas_event_freeze(evas_object_evas_get(iv->gui.img));
   if (iv->flags.next)
     {
	iv->flags.next = EINA_FALSE;

	if (iv->gui.next_img)
	  {
	     if (iv->files->next)
	       iv->files = iv->files->next;
	     else
	       iv->files = rewind_list(iv->files);

	     if (iv->gui.prev_img)
	       evas_object_del(iv->gui.prev_img);
	     iv->gui.prev_img = iv->gui.img;
	     evas_object_hide(iv->gui.img);

	     iv->gui.img = iv->gui.next_img;
	     image_configure(iv);
	     elm_label_label_set(iv->gui.file_label,
				 (char *) ecore_file_file_get(iv->files->data));
	     /* elm_scroller_content_set(iv->gui.scroller, iv->gui.img); */
	     elm_layout_content_set(iv->gui.ly, "iv.swallow.image", iv->gui.img);
	     evas_object_show(iv->gui.img);
	     iv->gui.next_img = NULL;
	  }
     }
   else if (iv->flags.prev)
     {
	iv->flags.prev = EINA_FALSE;

	if (iv->gui.prev_img)
	  {
	     if (iv->files->prev)
	       iv->files = iv->files->prev;
	     else
	       iv->files = eina_list_last(iv->files);

	     if (iv->gui.next_img)
	       evas_object_del(iv->gui.next_img);
	     iv->gui.next_img = iv->gui.img;
	     evas_object_hide(iv->gui.img);

	     iv->gui.img = iv->gui.prev_img;
	     image_configure(iv);
	     elm_label_label_set(iv->gui.file_label,
				 (char *) ecore_file_file_get(iv->files->data));
	     /* elm_scroller_content_set(iv->gui.scroller, iv->gui.img); */
	     elm_layout_content_set(iv->gui.ly, "iv.swallow.image", iv->gui.img);
	     evas_object_show(iv->gui.img);
	     iv->gui.prev_img = NULL;
	  }
     }
   evas_event_thaw(evas_object_evas_get(iv->gui.img));

   if (iv->flags.hide_controls)
     {
	iv->flags.hide_controls = EINA_FALSE;

	edje_object_signal_emit(elm_layout_edje_get(iv->gui.ly),
			       	"iv,state,hide_controls", "iv");
	elm_hoversel_hover_end(iv->gui.hoversel);
     }
   if (iv->flags.fit_changed)
     {
	iv->flags.fit_changed = EINA_FALSE;
	switch (iv->config.fit)
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
	  }
	if (iv->gui.img)
	  image_configure(iv);
     }

   return 1;
}

static void
on_hoversel_fs(void *data, Evas_Object *obj, void *event_info)
{
   IV *iv = data;

   if (!iv) return;
   iv->flags.fit_changed = EINA_TRUE;
   iv->config.fit = FIT_SCALE;
}

static void
on_hoversel_f(void *data, Evas_Object *obj, void *event_info)
{
   IV *iv = data;

   if (!iv) return;
   iv->flags.fit_changed = EINA_TRUE;
   iv->config.fit = FIT;
}

static void
on_hoversel_p(void *data, Evas_Object *obj, void *event_info)
{
   IV *iv = data;

   if (!iv) return;
   iv->flags.fit_changed = EINA_TRUE;
   iv->config.fit = PAN;
}

// main window - setup
static void
create_main_win(IV *iv)
{
   Evas_Object *o, *bx, *ic;
   char buf[4096];
   
   snprintf(buf, sizeof(buf), "%s/themes/default.edj", PACKAGE_DATA_DIR);

   elm_theme_overlay_add(buf);

   o = elm_win_add(NULL, "main", ELM_WIN_BASIC);
   elm_win_title_set(o, "Image Viewer");
   evas_object_smart_callback_add(o, "delete-request", on_win_del_req, NULL);
   evas_object_resize(o, 200, 180);
   iv->gui.win = o;

   o = elm_bg_add(iv->gui.win);
   elm_win_resize_object_add(iv->gui.win, o);
   evas_object_size_hint_weight_set(o, 1.0, 1.0);
   evas_object_show(o);
   
   /*
    * XXX: configurable themes
    * */
   o = elm_layout_add(iv->gui.win);
   elm_layout_file_set(o, buf, "iv/main");
   evas_object_size_hint_weight_set(o, 1.0, 1.0);
   elm_win_resize_object_add(iv->gui.win, o);
   evas_object_focus_set(o, 1);
   evas_object_event_callback_add(o, EVAS_CALLBACK_RESIZE, on_layout_resize, iv);
   evas_object_event_callback_add(o, EVAS_CALLBACK_KEY_DOWN, on_key_down, iv);
   edje_object_signal_callback_add(elm_layout_edje_get(o),
				   "iv,action,click", "", on_image_click, iv);
   evas_object_show(o);
   iv->gui.ly = o;

   /*
   o = elm_scroller_add(iv->gui.ly);
   evas_object_size_hint_weight_set(o, 1.0, 1.0);
   elm_win_resize_object_add(iv->gui.win, o);
   elm_layout_content_set(iv->gui.ly, "iv.swallow.image", o);
   iv->gui.scroller = o;
   */

   o = elm_layout_add(iv->gui.ly);
   elm_layout_file_set(o, buf, "iv/controls");
   evas_object_size_hint_weight_set(o, 1.0, 1.0);
   elm_layout_content_set(iv->gui.ly, "iv.swallow.controls", o);
   edje_object_signal_callback_add(elm_layout_edje_get(o),
				   "iv,action,click", "", on_controls_click, iv);
   evas_object_show(o);
   iv->gui.controls = o;

   bx = elm_box_add(iv->gui.controls);
   elm_box_horizontal_set(bx, 1);
   evas_object_size_hint_weight_set(bx, 1.0, 1.0);
   elm_layout_content_set(iv->gui.controls, "iv.swallow.box", bx);
   evas_object_show(bx);

   ic = elm_icon_add(bx);
#ifdef HAVE_ENLIGHTENMENT
   elm_icon_file_set(ic, efreet_icon_path_find(e_config->icon_theme,
					       "media-seek-backward", 32),
		     NULL);
#else
   elm_icon_file_set(ic, buf, "iv/controls/prev");
#endif
   elm_icon_scale_set(ic, 0, 0);
   evas_object_size_hint_align_set(ic, 0.0, 1.0);
   evas_object_show(ic);
   
   o = elm_button_add(bx);
   elm_button_icon_set(o, ic);
   evas_object_size_hint_align_set(o, 0.5, 0.5);
   elm_box_pack_end(bx, o);
   evas_object_smart_callback_add(o, "clicked", on_prev_click, iv);
   evas_object_show(o);
   iv->gui.prev_bt = o;

   ic = elm_icon_add(bx);
#ifdef HAVE_ENLIGHTENMENT
   elm_icon_file_set(ic, efreet_icon_path_find(e_config->icon_theme,
					       "media-seek-forward", 32),
		     NULL);
#else
   elm_icon_file_set(ic, buf, "iv/controls/next");
#endif
   elm_icon_scale_set(ic, 0, 0);
   evas_object_size_hint_align_set(ic, 0.0, 1.0);
   evas_object_show(ic);
   
   o = elm_button_add(bx);
   elm_button_icon_set(o, ic);
   evas_object_size_hint_align_set(o, 0.5, 0.5);
   elm_box_pack_end(bx, o);
   evas_object_smart_callback_add(o, "clicked", on_next_click, iv);
   evas_object_show(o);
   iv->gui.next_bt = o;

   o = elm_label_add(bx);
   elm_box_pack_end(bx, o);
   evas_object_show(o);
   iv->gui.file_label = o;

   o = elm_hoversel_add(bx);
   elm_box_pack_end(bx, o);
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

   free(iv);
}

static void
init_config(IV *iv)
{
   iv->config.fit = FIT;
}

EAPI int
elm_main(int argc, char **argv)
{
   int i;
   IV *iv;
   
   iv = calloc(1, sizeof(IV));
   init_config(iv);

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

   iv->flags.fit_changed = EINA_TRUE;
   iv->idle_enterer = ecore_idle_enterer_add(on_idle_enterer, iv);

   elm_run();

   iv_free(iv);

   elm_shutdown();
   return 0; 
}

ELM_MAIN()
