#include <Eet.h>
#include <Ecore.h>
#include <Ecore_X.h>
#include <Ecore_File.h>
#include <Elementary.h>
#include <Efreet.h>
#include <Efreet_Trash.h>

#include "config.h"

#define CONFIG_VERSION 1

#ifdef HAVE_ETHUMB
  #include <Ethumb_Client.h>
#endif

#ifdef HAVE_LIBEXIF
  #include <libexif/exif-data.h>
#endif

static int __log_domain = -1;
#define DBG(...) EINA_LOG_DOM_DBG(__log_domain, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(__log_domain, __VA_ARGS__)

#define IV_FILE(o) ((IV_File *) o)
#define IV_FILE_NEXT(o) IV_FILE(EINA_INLIST_GET(o)->next)
#define IV_FILE_PREV(o) IV_FILE(EINA_INLIST_GET(o)->prev)
#define IV_FILE_LAST(o) IV_FILE(EINA_INLIST_GET(o)->last)

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
typedef struct _IV_File IV_File;
typedef struct _IV_Files_Account IV_Files_Account;

struct _IV
{
   Eina_Inlist *files;
   IV_Files_Account *account;

   Eina_List  *dirs, *file_monitors;
   const char *single_file;
   const char *theme_file;

   Ecore_Idler *idler;

   Ecore_Timer *slideshow_timer, *cursor_timer;

   Ecore_Event_Handler *on_win_move_handler;

   struct {
	Evas_Object *win, *ly, *img, *scroller, *prev_img, *next_img, *bg;
	Evas_Object *controls, *file_label, *next_bt, *prev_bt;
        Evas_Object *hoversel, *settings_bt, *slideshow_bt, *settings_win;
#ifdef HAVE_ETHUMB
	Evas_Object *preview_win, *preview_genlist;
#endif

	struct {
	     Evas_Coord x, y, w, h;
	} region;

	struct {
	     Evas_Coord w, h;
	} image_size;
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
	Eina_Bool hide_previews : 1;
	Eina_Bool previews_visible : 1;
	Eina_Bool ethumb_connection : 1;
#endif
   } flags;

   IV_Config            *config;
   Eet_Data_Descriptor  *config_edd;
   Ecore_Timer          *config_save;

#ifdef HAVE_ETHUMB
   Eina_List		*preview_files;
   Ethumb_Client        *ethumb_client;

   int			 connection_retry;

   int			 log_domain;

   Elm_Genlist_Item_Class *itc;
#endif
};

struct _IV_Config {
     int          config_version;

     int	  auto_hide_previews;
     double       img_scale;
     double	  slideshow_delay;
     const char  *image_editor;
     IV_Image_Fit fit;
     IV_Image_Bg  image_bg;
};

struct _IV_File
{
   EINA_INLIST;
   IV *iv;
   IV_Files_Account *account;

   const char *file_path;
#ifdef HAVE_ETHUMB
   const char *thumb_path;
   Elm_Genlist_Item *gl_item;
#endif

   /* for adding new files */
   IV_File *insert_before;
   double change_time;

   struct {
	Eina_Bool changed : 1;
#ifdef HAVE_ETHUMB
	Eina_Bool thumb_generating : 1;
#endif
   } flags;
};

struct _IV_Files_Account
{
   int count;

   IV_File *current;
};

/* Prototypes */
static Eina_Bool on_idler(void *data);
static void slideshow_on(IV *iv);
static void slideshow_off(IV *iv);
#ifdef HAVE_ETHUMB
static void on_thumb_generate(void *data, Ethumb_Client *client, int id, const char *file_path, const char *key, const char *thumb_path, const char *thumb_key, Eina_Bool success);
static void on_thumb_die(void *data, Ethumb_Client *client);
static void on_preview_close_click(void *data, Evas_Object *obj, void *event_info);
#endif

static void
image_configure(IV *iv)
{
   if (!iv->gui.image_size.w || !iv->gui.image_size.h)
     {
	elm_image_scale_set(iv->gui.img, EINA_FALSE, EINA_FALSE);
	evas_object_size_hint_max_get(iv->gui.img, &(iv->gui.image_size.w), &(iv->gui.image_size.h));
     }

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
}

static void
set_image_bg_style(IV *iv)
{
   switch(iv->config->image_bg)
     {
      case IMAGE_BG_BLACK:
	 elm_object_style_set(iv->gui.bg, "black");
	 break;
      case IMAGE_BG_CHECKERS:
	 elm_object_style_set(iv->gui.bg, "checkers");
	 break;
     }
}

static IV_File *
create_iv_file(IV *iv, const char *path)
{
   IV_File *file = calloc(1, sizeof(IV_File));

   file->iv = iv;
   file->file_path = eina_stringshare_add(path);
   file->account = iv->account;

   iv->account->count++;

   return file;
}

static void
remove_iv_file(IV *iv, IV_File *file, Eina_Bool set_current)
{
   if (iv->account->current == file && set_current)
     {
	if (IV_FILE_NEXT(file))
	  iv->account->current = IV_FILE_NEXT(file);
	else
	  iv->account->current = IV_FILE(iv->files);
     }
   iv->files = eina_inlist_remove(iv->files, EINA_INLIST_GET(file));
#ifdef HAVE_ETHUMB
   iv->preview_files = eina_list_remove(iv->preview_files, file);
#endif

   iv->account->count--;

   eina_stringshare_del(file->file_path);
#ifdef HAVE_ETHUMB
   eina_stringshare_del(file->thumb_path);
#endif
   free(file);
}

static Eina_Bool
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

static Eina_Bool
on_win_move_tick(void *data)
{
   IV *iv = data;

   iv->cursor_timer = NULL;
   if (iv->flags.fullscreen)
     ecore_x_window_cursor_show(elm_win_xwindow_get(iv->gui.win), 0);
   
   return ECORE_CALLBACK_CANCEL;
}

static Eina_Bool
on_win_move(void *data, int event_type, void *event_info)
{
   IV *iv = data;
   Ecore_Event_Mouse_Move *ev = event_info;

   if (ev->window != elm_win_xwindow_get(iv->gui.win)) return 1;
   if (iv->cursor_timer)
     ecore_timer_del(iv->cursor_timer);
   iv->cursor_timer = ecore_timer_add(1.0, on_win_move_tick, iv);
   ecore_x_window_cursor_show(elm_win_xwindow_get(iv->gui.win), 1);
   return 1;
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
on_image_click(void *data, Evas *a, Evas_Object *obj, void *event_info)
{
   IV *iv = data;
   Evas_Event_Mouse_Down *ev = event_info;

   if (iv->config->fit == PAN || iv->config->fit == ZOOM)
     return;

   if (ev->button == 3)
     iv->flags.prev = EINA_TRUE;
   if (ev->button== 1)
     iv->flags.next = EINA_TRUE;  

   iv->flags.hide_controls = EINA_TRUE;
   if (!iv->idler)
     iv->idler = ecore_idler_add(on_idler, iv);
}

static void
on_settings_slideshow_delay_change(void *data, Evas_Object *obj, void *event_info)
{
   IV *iv = data;

   iv->config->slideshow_delay = elm_spinner_value_get(obj);
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
on_settings_edit_entry_change(void *data, Evas_Object *obj, void *event_info)
{
   IV *iv = data;
   char *txt;
   int len;

   txt = elm_entry_markup_to_utf8(elm_entry_entry_get(obj));
   if (txt)
     {
       len = strlen(txt);
       if (txt[len - 1] == '\n') txt[len - 1] = '\0';
       iv->config->image_editor = eina_stringshare_add(txt);

       free(txt);

       config_save(iv);
     }
}

static void
on_settings_close_click(void *data, Evas_Object *obj, void *event_info)
{
   IV *iv = data;

   evas_object_hide(iv->gui.settings_win);
   evas_object_focus_set(iv->gui.ly, 1);
}

static void
record_visible_region(IV *iv)
{
   elm_scroller_region_get(iv->gui.scroller,
			   &(iv->gui.region.x), &(iv->gui.region.y),
			   &(iv->gui.region.w), &(iv->gui.region.h));
}

static void
zoom_set(IV *iv, Eina_Bool increase)
{
   double scale;

   if (iv->config->fit != ZOOM)
     {
	Evas_Coord w;
	scale = elm_object_scale_get(iv->gui.img);

	evas_object_geometry_get(iv->gui.img, NULL, NULL, &w, NULL);

	if (w > iv->gui.image_size.w)
	  iv->config->img_scale = scale;
	else
	  {
	     if (scale == 1.0)
	       iv->config->img_scale = (double) w / iv->gui.image_size.w;
	     else
	       iv->config->img_scale = scale;
	  }
     }

   record_visible_region(iv);
   scale = iv->config->img_scale;
   if (increase)
     iv->config->img_scale = iv->config->img_scale * 1.1;
   else
     iv->config->img_scale = iv->config->img_scale / 1.1;

   if (iv->config->img_scale > 0.97 && iv->config->img_scale < 1.04)
     iv->config->img_scale = 1.0;
   else if (iv->config->img_scale < 0.01)
     iv->config->img_scale = 0.01;

   iv->gui.region.x *=  (double) iv->config->img_scale / scale;
   iv->gui.region.y *=  (double) iv->config->img_scale / scale;
   if (iv->gui.region.x < 0)
     iv->gui.region.x = 0;
   if (iv->gui.region.y < 0)
     iv->gui.region.y = 0;
   elm_scroller_region_show(iv->gui.scroller, iv->gui.region.x, iv->gui.region.y, iv->gui.region.w, iv->gui.region.h);

   iv->config->fit = ZOOM;
   iv->flags.fit_changed = EINA_TRUE;
   if (!iv->idler)
     iv->idler = ecore_idler_add(on_idler, iv);
   config_save(iv);
}

static void
fullscreen(IV *iv)
{
   iv->flags.fullscreen = EINA_TRUE;
   elm_win_fullscreen_set(iv->gui.win, 1);

   ecore_x_window_cursor_show(elm_win_xwindow_get(iv->gui.win), 0);
   iv->on_win_move_handler = ecore_event_handler_add(ECORE_EVENT_MOUSE_MOVE, on_win_move, iv);
}

static void
unfullscreen(IV *iv)
{
   iv->flags.fullscreen = EINA_FALSE;
   elm_win_fullscreen_set(iv->gui.win, 0);

   ecore_x_window_cursor_show(elm_win_xwindow_get(iv->gui.win), 1);
   ecore_event_handler_del(iv->on_win_move_handler);
}

static void
set_image_text(IV *iv, IV_File *file)
{
   char buf[1024];
   const char *text = ecore_file_file_get(file->file_path);

   edje_object_part_text_set(iv->gui.file_label, "iv.text.label", text);

   snprintf(buf, sizeof(buf), "Image Viewer - %s", text);
   elm_win_title_set(iv->gui.win, buf);
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

#ifdef HAVE_ETHUMB
char *iv_gl_label_get(const void *data, Evas_Object *obj, const char *part)
{
   const IV_File *file = data;
   if (!strcmp(part, "elm.text"))
     return strdup(ecore_file_file_get(file->file_path));
   else if (!strcmp(part, "elm.text.sub"))
     return ecore_file_dir_get(file->file_path);

   return NULL;
}

Evas_Object *iv_gl_icon_get(const void *data, Evas_Object *obj, const char *part)
{
   const IV_File *file = data;
   if (!strcmp(part, "elm.swallow.icon"))
     {
        Evas_Object *ic = elm_icon_add(obj);
        elm_icon_file_set(ic, file->thumb_path, NULL);
        elm_icon_scale_set(ic, 0, 1);
	evas_object_size_hint_min_set(ic, 64, 64);
        evas_object_show(ic);
        return ic;
     }
   return NULL;
}

Eina_Bool iv_gl_state_get(const void *data, Evas_Object *obj, const char *part)
{
   return 0;
}

static void
preview_window_create(IV *iv)
{
   Evas_Object *bx, *bt, *ic;

   iv->gui.preview_win = elm_win_inwin_add(iv->gui.win);
   elm_object_style_set(iv->gui.preview_win, "shadow_fill");
   evas_object_size_hint_weight_set(elm_bg_add(iv->gui.preview_win), 1.0, 1.0);

   bx = elm_box_add(iv->gui.preview_win);
   evas_object_show(bx);

   iv->gui.preview_genlist = elm_genlist_add(bx);
   elm_genlist_always_select_mode_set(iv->gui.preview_genlist, 1);
   evas_object_size_hint_align_set(iv->gui.preview_genlist, -1.0, -1.0);
   evas_object_size_hint_weight_set(iv->gui.preview_genlist, 1.0, 1.0);
   evas_object_show(iv->gui.preview_genlist);

   iv->itc = calloc(1, sizeof(Elm_Genlist_Item_Class));
   iv->itc->item_style     = "double_label";
   iv->itc->func.label_get = iv_gl_label_get;
   iv->itc->func.icon_get  = iv_gl_icon_get;
   iv->itc->func.state_get = iv_gl_state_get;
   iv->itc->func.del       = NULL;

   ic = elm_icon_add(bx);
   elm_icon_standard_set(ic, "close");
   bt = elm_button_add(bx);
   elm_icon_scale_set(ic, 0, 0);
   elm_button_icon_set(bt, ic);
   elm_button_label_set(bt, "Close");
   evas_object_smart_callback_add(bt, "clicked",
				  on_preview_close_click, iv);
   evas_object_show(bt);

   elm_box_pack_start(bx, iv->gui.preview_genlist);
   elm_box_pack_end(bx, bt);
   elm_win_inwin_content_set(iv->gui.preview_win, bx);
}

static void
toggle_previews(IV *iv)
{
   if (!iv->ethumb_client) return;
   if (!iv->gui.preview_genlist)
     preview_window_create(iv);

   if (evas_object_visible_get(iv->gui.preview_win))
     evas_object_hide(iv->gui.preview_win);
   else
     {
	elm_win_inwin_activate(iv->gui.preview_win);
	if (!iv->idler)
	  iv->idler = ecore_idler_add(on_idler, iv);
     }
}

static void
on_preview_close_click(void *data, Evas_Object *obj, void *event_info)
{
   IV *iv = data;

   evas_object_hide(iv->gui.preview_win);
}
static void
on_thumb_sel(void *data, Evas_Object *obj, void *event_info)
{
   IV_File *file = data;
   IV *iv = file->iv;

   elm_genlist_item_show(file->gl_item);
   iv->account->current = file;
   iv->flags.current = EINA_TRUE;
   if (iv->config->auto_hide_previews)
     iv->flags.hide_previews = EINA_TRUE;
   if (!iv->idler)
     iv->idler = ecore_idler_add(on_idler, iv);
}

static void
thumb_remove(IV_File *file)
{
   eina_stringshare_del(file->thumb_path);
   elm_genlist_item_del(file->gl_item);

   file->thumb_path = NULL;
   file->gl_item = NULL;
}

static void
thumb_queue_process(IV *iv)
{
   IV_File *file;

   EINA_LIST_FREE(iv->preview_files, file)
     {
	if (!ethumb_client_file_set(iv->ethumb_client, file->file_path, NULL))
	  continue;

	if (ethumb_client_thumb_exists(iv->ethumb_client))
	  {
	     const char *thumb_path;

	     ethumb_client_thumb_path_get(iv->ethumb_client, &thumb_path, NULL);
	     on_thumb_generate(file, iv->ethumb_client, 0, file->file_path, NULL, thumb_path, NULL, EINA_TRUE);
	  }
	else
	  {
	     file->flags.thumb_generating = 1;
	     if (ethumb_client_generate(iv->ethumb_client, on_thumb_generate, file, NULL) == -1)
	       continue;
	  }
     }
}

static void
on_thumb_generate(void *data, Ethumb_Client *client, int id, const char *file_path, const char *key, const char *thumb_path, const char *thumb_key, Eina_Bool success)
{
   IV_File *file = data;

   file->flags.thumb_generating = 0;
   if (!success) return;

   file->thumb_path = eina_stringshare_add(thumb_path);
   if (file->insert_before)
     file->gl_item = elm_genlist_item_insert_before(
	 file->iv->gui.preview_genlist, file->iv->itc, file,
	 file->insert_before->gl_item, ELM_GENLIST_ITEM_NONE,
	 on_thumb_sel, file);
   else
     file->gl_item = elm_genlist_item_append(
	 file->iv->gui.preview_genlist, file->iv->itc, file, NULL,
	 ELM_GENLIST_ITEM_NONE, on_thumb_sel, file);

   if (file->account->current == file)
     elm_genlist_item_selected_set(file->gl_item, EINA_TRUE);
}

static void
on_thumb_connect(void *data, Ethumb_Client *e, Eina_Bool success)
{
   IV *iv = data;

   if (!success)
     {
	iv->flags.ethumb_connection = EINA_FALSE;
	iv->connection_retry--;
	iv->ethumb_client = NULL;
	ERR("Error connecting to ethumbd, thumbnails will not be available!");
	return;
     }

   iv->flags.ethumb_connection = EINA_TRUE;
   if (!iv->gui.preview_genlist)
     preview_window_create(iv);

   ethumb_client_on_server_die_callback_set(iv->ethumb_client, on_thumb_die, iv, NULL);
   thumb_queue_process(iv);
}

static void
on_thumb_die(void *data, Ethumb_Client *client)
{
   IV *iv = data;

   iv->flags.ethumb_connection = EINA_FALSE;
   iv->ethumb_client = ethumb_client_connect(on_thumb_connect, iv, NULL);
   ERR("Connection to ethumbd lost!");
}

static void
on_show_previews(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   IV *iv = data;

   if (iv->gui.preview_win && (!evas_object_visible_get(iv->gui.preview_win)))
     toggle_previews(iv);
}
#endif

static void
on_file_monitor_event(void *data, Ecore_File_Monitor *em, Ecore_File_Event event, const char *path)
{
   IV *iv = data;
   IV_File *file;
   const char *cur_path;
   char *dir = NULL;

   cur_path = iv->account->current->file_path;
   switch(event)
     {
      case ECORE_FILE_EVENT_CREATED_FILE:
      case ECORE_FILE_EVENT_MODIFIED:
	 dir = ecore_file_dir_get(path);

	 EINA_INLIST_FOREACH(iv->files, file)
	   {
	      if (strncmp(dir, file->file_path, strlen(dir)))
		continue;

	      if (!strcmp(path, file->file_path))
		{
#ifdef HAVE_ETHUMB
		   if (file->thumb_path)
		     thumb_remove(file);
		   if (!eina_list_data_find_list(iv->preview_files, file) && !file->flags.thumb_generating)
		     {
			if (IV_FILE_NEXT(file))
			  file->insert_before = IV_FILE_NEXT(file);
			iv->preview_files = eina_list_append(iv->preview_files, file);
		     }
#endif
		   file->change_time = ecore_time_unix_get();
		   file->flags.changed = 1;
		   iv->flags.current = EINA_TRUE;
		   break;
		}
	      else if (strcmp(path, file->file_path) < 0)
		{
		   IV_File *new = create_iv_file(iv, path);
		   iv->files = eina_inlist_prepend_relative(iv->files, EINA_INLIST_GET(new), EINA_INLIST_GET(file));
#ifdef HAVE_ETHUMB
		   new->insert_before = file;
		   iv->preview_files = eina_list_append(iv->preview_files, new);
#endif
		   new->change_time = ecore_time_unix_get();
		   new->flags.changed = 1;
		   iv->flags.current = EINA_TRUE;
		   break;
		}
	   }
	 if (!iv->flags.current)
	   {
	      IV_File *new = create_iv_file(iv, path);
	      new->change_time = ecore_time_unix_get();
	      new->flags.changed = 1;
	      iv->files = eina_inlist_append(iv->files, EINA_INLIST_GET(new));
#ifdef HAVE_ETHUMB
	      iv->preview_files = eina_list_append(iv->preview_files, new);
#endif
	   }
	 free(dir);
	 break;
      case ECORE_FILE_EVENT_DELETED_FILE:
	 EINA_INLIST_FOREACH(iv->files, file)
	   {
	      if (!strcmp(path, file->file_path))
		{
#ifdef HAVE_ETHUMB
		   if (file->thumb_path)
		     thumb_remove(file);
#endif
		   remove_iv_file(iv, file, EINA_TRUE);
		   iv->flags.current = EINA_TRUE;
		   break;
		}
	   }
	 break;
      case ECORE_FILE_EVENT_DELETED_SELF:
	 EINA_INLIST_FOREACH(iv->files, file)
	   {
	      if (!strncmp(path, file->file_path, strlen(path)))
		{
		   if (file->file_path == cur_path)
		     cur_path = NULL;
#ifdef HAVE_ETHUMB
		   if (file->thumb_path)
		     thumb_remove(file);
#endif
		   remove_iv_file(iv, file, EINA_TRUE);
		   iv->flags.current = EINA_TRUE;
		}
	   }
	 break;
      default:
	 break;
     }

   if (!iv->idler && iv->flags.current)
     iv->idler = ecore_idler_add(on_idler, iv);
}

static void
read_image(IV *iv, IV_Image_Dest dest)
{
   Evas_Object *img;
   IV_File *f;

   switch (dest)
     {
      case IMAGE_CURRENT:
	 f = iv->account->current;
	 break;
      case IMAGE_NEXT:
	 f = IV_FILE_NEXT(iv->account->current);

	 if (!f)
	   {
	      f = IV_FILE(iv->files);
	      if (f == iv->account->current)
		f = NULL;
	   }
	 break;
      case IMAGE_PREV:
	 f = IV_FILE_PREV(iv->account->current);

	 if (!f)
	   {
	      f = IV_FILE_LAST(iv->account->current);
	      if (f == iv->account->current)
		f = NULL;
	   }
	 break;
     }

   while (f)
     {
	Eina_Bool succ = EINA_FALSE;

	img = elm_image_add(iv->gui.ly);
	succ = elm_image_file_set(img, f->file_path, NULL);
	if (succ)
	  {
	     int orientation = 0;
#ifdef HAVE_LIBEXIF
	     ExifData  *exif = exif_data_new_from_file(f->file_path);
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

	     switch (dest)
	       {
		case IMAGE_CURRENT:
		   iv->gui.img = img;
		   image_configure(iv);
		   set_image_text(iv, f);
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
	     evas_object_event_callback_add(img, EVAS_CALLBACK_MOUSE_DOWN, on_image_click, iv);
	     f->flags.changed = 0;
	     break;
	  }
	else
	  {
	     IV_File *cur = f;
	     Eina_Bool same_file = 0;
	     switch (dest)
	       {
		case IMAGE_NEXT:
		case IMAGE_CURRENT:
		   f = IV_FILE_NEXT(iv->account->current) ?
		      IV_FILE_NEXT(iv->account->current) :
		      IV_FILE_PREV(iv->account->current);
		   break;
		case IMAGE_PREV:
		   f = IV_FILE_PREV(iv->account->current) ?
		      IV_FILE_PREV(iv->account->current) :
		      IV_FILE_LAST(iv->account->current);
		   break;
	       }
	     if (cur->flags.changed)
	       {
		  if (f->change_time - ecore_time_unix_get() > 60)
		    f->flags.changed = 0;
		  break;
	       }
#ifdef HAVE_ETHUMB
	     if (cur->thumb_path)
	       thumb_remove(cur);
	     if (iv->preview_files)
	       {
		  Eina_List *l = eina_list_data_find_list(iv->preview_files, cur);
		  if (l)
		    iv->preview_files = eina_list_remove_list(iv->preview_files, l);
	       }
#endif
	     /* If the new file is the same as the current one, we ran out of files */
	     if (cur == f)
	       same_file = 1;
	     else if (dest == IMAGE_CURRENT)
	       iv->account->current = f;

	     remove_iv_file(iv, cur, EINA_FALSE);
	     evas_object_del(img);

	     if (same_file) break;
	  }
     }
}

static void
trash_image(IV *iv)
{
   Efreet_Uri *uri;
   char buf[4096];

   if (!iv->account->current)
     return;

   snprintf(buf, sizeof(buf), "file://%s", iv->account->current->file_path);
   uri = efreet_uri_decode(buf);

   if (uri)
     {
	IV_File *cur = iv->account->current;
	remove_iv_file(iv, cur, EINA_TRUE);
	iv->flags.current = EINA_TRUE;
	efreet_trash_delete_uri(uri, 0);
	efreet_uri_free(uri);
     }
}

static Eina_Bool
on_idler(void *data)
{
   IV *iv = data;
   Eina_Bool renew = EINA_FALSE;

   if (iv->dirs)
     {
	Eina_List *files;
	char *dir, *file, buf[4096], buf2[4096];

	dir = iv->dirs->data;

	iv->dirs = eina_list_remove_list(iv->dirs, iv->dirs);
	files = ecore_file_ls(dir);
	if (eina_str_has_suffix(dir, "/"))
	  snprintf(buf2, sizeof(buf2), "%s", dir);
	else
	  snprintf(buf2, sizeof(buf2), "%s/", dir);

	eina_stringshare_del(dir);

	EINA_LIST_FREE(files, file)
	  {
	     snprintf(buf, sizeof(buf), "%s%s", buf2, file);
	     free(file);
	     /* XXX: recursive scanning with an option */
	     if (!ecore_file_is_dir(buf))
	       {
		  IV_File *new = create_iv_file(iv, buf);
		  iv->files = eina_inlist_append(iv->files, EINA_INLIST_GET(new));

		  if (iv->single_file && iv->single_file == new->file_path)
		    {
		       iv->account->current = new;
		       iv->single_file = NULL;
		    }
#ifdef HAVE_ETHUMB
		  iv->preview_files = eina_list_append(iv->preview_files, new);
#endif
	       }
	  }

	iv->file_monitors = eina_list_append(
	    iv->file_monitors,
	    ecore_file_monitor_add(buf2, on_file_monitor_event, iv));

	if (!renew && iv->dirs)
	  renew = EINA_TRUE;

	if (!iv->account->current) iv->account->current = IV_FILE(iv->files);
     }

   /* Display the first image */
   if (iv->account->current)
     {
	if (!iv->gui.img)
	  read_image(iv, IMAGE_CURRENT);
	else
	  {
	     if (IV_FILE_NEXT(iv->account->current) ||
		 IV_FILE_PREV(iv->account->current))
	       {
		  if (!iv->gui.prev_img)
		    {
		       read_image(iv, IMAGE_PREV);

		       if (iv->gui.prev_img)
			 elm_object_disabled_set(iv->gui.prev_bt, 0);
		       else
			 elm_object_disabled_set(iv->gui.prev_bt, 1);
		    }
		  if (!iv->gui.next_img)
		    {
		       read_image(iv, IMAGE_NEXT);

		       if (iv->gui.next_img)
			 elm_object_disabled_set(iv->gui.next_bt, 0);
		       else
			 elm_object_disabled_set(iv->gui.next_bt, 1);
		    }
	       }

#ifdef HAVE_ETHUMB
	     if (iv->preview_files && iv->connection_retry)
	       {
		  if (iv->ethumb_client && iv->flags.ethumb_connection)
		    thumb_queue_process(iv);
		  else if (!iv->ethumb_client)
		    iv->ethumb_client = ethumb_client_connect(on_thumb_connect, iv, NULL);
	       }
#endif
	  }
     }

   if (!renew && iv->account->current && 
       (!iv->gui.img || ((IV_FILE_PREV(iv->account->current) ||
			  IV_FILE_NEXT(iv->account->current)) &&
			 (!iv->gui.prev_img || !iv->gui.next_img))))
     renew = EINA_TRUE;

   if (iv->flags.next)
     {
	if (iv->gui.next_img)
	  {
	     iv->flags.next = EINA_FALSE;

	     iv->account->current = IV_FILE_NEXT(iv->account->current) ?
		IV_FILE_NEXT(iv->account->current) : IV_FILE(iv->files);

	     record_visible_region(iv);

	     /* XXX: this is necessary for some reason, bug in elm? */
	     elm_scroller_content_set(iv->gui.scroller, NULL);

	     if (iv->gui.prev_img)
	       {
		  evas_object_event_callback_del(iv->gui.prev_img, EVAS_CALLBACK_MOUSE_DOWN, on_image_click);
		  evas_object_del(iv->gui.prev_img);
	       }
	     iv->gui.prev_img = iv->gui.img;
	     evas_object_hide(iv->gui.img);

	     iv->gui.img = iv->gui.next_img;
	     image_configure(iv);
	     set_image_text(iv, iv->account->current);
	     elm_scroller_content_set(iv->gui.scroller, iv->gui.img);
	     elm_scroller_region_show(iv->gui.scroller, iv->gui.region.x, iv->gui.region.y, iv->gui.region.w, iv->gui.region.h);
	     evas_object_show(iv->gui.img);
	     iv->gui.next_img = NULL;

#ifdef HAVE_ETHUMB
	     if (iv->account->current->gl_item)
	       elm_genlist_item_selected_set(iv->account->current->gl_item,
					     EINA_TRUE);
#endif
	  }
     }
   else if (iv->flags.prev)
     {
	if (iv->gui.prev_img)
	  {
	     iv->flags.prev = EINA_FALSE;

	     iv->account->current = IV_FILE_PREV(iv->account->current) ?
		IV_FILE_PREV(iv->account->current) : IV_FILE(iv->files->last);

	     record_visible_region(iv);

	     /* XXX: this is necessary for some reason, bug in elm? */
	     elm_scroller_content_set(iv->gui.scroller, NULL);

	     if (iv->gui.next_img)
	       {
		  evas_object_event_callback_del(iv->gui.next_img, EVAS_CALLBACK_MOUSE_DOWN, on_image_click);
		  evas_object_del(iv->gui.next_img);
	       }
	     iv->gui.next_img = iv->gui.img;
	     evas_object_hide(iv->gui.img);

	     iv->gui.img = iv->gui.prev_img;
	     image_configure(iv);
	     set_image_text(iv, iv->account->current);
	     elm_scroller_content_set(iv->gui.scroller, iv->gui.img);
	     elm_scroller_region_show(iv->gui.scroller, iv->gui.region.x, iv->gui.region.y, iv->gui.region.w, iv->gui.region.h);
	     evas_object_show(iv->gui.img);
	     iv->gui.prev_img = NULL;

#ifdef HAVE_ETHUMB
	     if (iv->account->current->gl_item)
	       elm_genlist_item_selected_set(iv->account->current->gl_item,
					     EINA_TRUE);
#endif
	  }
     }
   else if (iv->flags.current)
     {
	iv->flags.current = EINA_FALSE;

	if (iv->gui.prev_img)
	  {
	     evas_object_event_callback_del(iv->gui.prev_img, EVAS_CALLBACK_MOUSE_DOWN, on_image_click);
	     evas_object_del(iv->gui.prev_img);
	     iv->gui.prev_img = NULL;
	  }
	if (iv->gui.next_img)
	  {
	     evas_object_event_callback_del(iv->gui.next_img, EVAS_CALLBACK_MOUSE_DOWN, on_image_click);
	     evas_object_del(iv->gui.next_img);
	     iv->gui.next_img = NULL;
	  }

	if (iv->gui.img)
	  {
	     evas_object_event_callback_del(iv->gui.img, EVAS_CALLBACK_MOUSE_DOWN, on_image_click);
	     evas_object_del(iv->gui.img);
	     iv->gui.img = NULL;
	  }

	record_visible_region(iv);

	/* XXX: this is necessary for some reason, bug in elm? */
	elm_scroller_content_set(iv->gui.scroller, NULL);

	read_image(iv, IMAGE_CURRENT);
	renew = EINA_TRUE;
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

	toggle_previews(iv);
     }
#endif
   if (iv->flags.fit_changed)
     {
	iv->flags.fit_changed = EINA_FALSE;
	switch (iv->config->fit)
	  {
	   case PAN:
	      elm_hoversel_label_set(iv->gui.hoversel, "Pan");
	   case ZOOM:
	      elm_scroller_bounce_set(iv->gui.scroller, 1, 1);
	      break;
	   case FIT:
	      elm_hoversel_label_set(iv->gui.hoversel, "Fit");
	      elm_scroller_bounce_set(iv->gui.scroller, 0, 0);
	      break;
	   case FIT_SCALE:
	      elm_hoversel_label_set(iv->gui.hoversel, "Fit & Scale");
	      elm_scroller_bounce_set(iv->gui.scroller, 0, 0);
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

static Eina_Bool
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
	Evas_Object *o, *tb, *ic, *sc;
	int row = 0;

	iv->gui.settings_win = o = elm_win_inwin_add(iv->gui.win);
	elm_object_style_set(o, "shadow");
	o = elm_bg_add(iv->gui.settings_win);
	evas_object_size_hint_weight_set(o, 1.0, 1.0);

	tb = elm_table_add(iv->gui.settings_win);
	evas_object_show(tb);

	o = elm_label_add(tb);
	elm_label_label_set(o, "Slideshow: image delay");
	evas_object_size_hint_align_set(o, 0.9, 0.5);
	elm_table_pack(tb, o, 0, row, 1, 1);
	evas_object_show(o);

	o = elm_spinner_add(tb);
	evas_object_size_hint_weight_set(o, 1.0, 1.0);
	evas_object_size_hint_align_set(o, -1, -1);
	elm_spinner_step_set(o, 0.1);
	elm_spinner_min_max_set(o, 1.0, 10.0);
	elm_spinner_label_format_set(o, "%1.1f seconds");
	elm_spinner_value_set(o, iv->config->slideshow_delay);
	evas_object_smart_callback_add(o, "delay,changed",
				       on_settings_slideshow_delay_change, iv);
	evas_object_show(o);
	elm_table_pack(tb, o, 1, row++, 1, 1);

	o = elm_label_add(tb);
	elm_label_label_set(o, "Image background");
	evas_object_size_hint_align_set(o, 0.9, 0.5);
	elm_table_pack(tb, o, 0, row, 1, 1);
	evas_object_show(o);

	o = elm_toggle_add(tb);
	evas_object_size_hint_align_set(o, -1, -1);
	elm_toggle_states_labels_set(o, "checkers", "black");
        if (iv->config->image_bg == IMAGE_BG_BLACK)
          elm_toggle_state_set(o, 0);
        else
          elm_toggle_state_set(o, 1);
	evas_object_smart_callback_add(o, "changed",
				       on_settings_bg_toggle_change, iv);
	elm_table_pack(tb, o, 1, row++, 1, 1);
	evas_object_show(o);

#ifdef HAVE_ETHUMB
	o = elm_label_add(tb);
	elm_label_label_set(o, "Auto-hide the preview pane");
	evas_object_size_hint_align_set(o, 0.9, 0.5);
	elm_table_pack(tb, o, 0, row, 1, 1);
	evas_object_show(o);

	o = elm_toggle_add(tb);
	evas_object_size_hint_align_set(o, -1, -1);
	elm_toggle_state_set(o, iv->config->auto_hide_previews);
	evas_object_smart_callback_add(o, "changed",
				       on_settings_auto_hide_previews_toggle_change, iv);
	elm_table_pack(tb, o, 1, row++, 1, 1);
	evas_object_show(o);
#endif

	o = elm_label_add(tb);
	elm_label_label_set(o, "Image editor command");
	evas_object_size_hint_align_set(o, 0.9, 0.5);
	elm_table_pack(tb, o, 0, row, 1, 1);
	evas_object_show(o);

	sc = elm_scroller_add(tb);
	elm_scroller_policy_set(sc, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);
	evas_object_size_hint_weight_set(sc, 1.0, 1.0);
	evas_object_size_hint_align_set(sc, -1, -1);
	elm_table_pack(tb, sc, 1, row++, 1, 1);
	elm_scroller_bounce_set(sc, 0, 0);
	evas_object_show(sc);

	o = elm_entry_add(tb);
	elm_entry_entry_set(o, iv->config->image_editor);
	evas_object_size_hint_weight_set(o, 1.0, 1.0);
	evas_object_size_hint_align_set(o, -1.0, -1.0);
	evas_object_smart_callback_add(o, "changed",
				       on_settings_edit_entry_change, iv);
	elm_scroller_content_set(sc, o);
	evas_object_show(o);

	ic = elm_icon_add(tb);
	elm_icon_standard_set(ic, "close");
	o = elm_button_add(tb);
	elm_icon_scale_set(ic, 0, 0);
	elm_button_icon_set(o, ic);
	elm_button_label_set(o, "Close");
	evas_object_smart_callback_add(o, "clicked",
				       on_settings_close_click, iv);
	elm_table_pack(tb, o, 0, row++, 2, 1);
	evas_object_show(o);

	elm_win_inwin_content_set(iv->gui.settings_win, tb);
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
   else if (!strcmp(ev->keyname, "Delete"))
     trash_image(iv);
   else if (!strcmp(ev->keyname, "F12"))
     on_settings_click(iv, NULL, NULL);
#ifdef HAVE_ETHUMB
   else if (!strcmp(ev->keyname, "F9"))
     toggle_previews(iv);
#endif
   else if (!strcmp(ev->keyname, "e"))
     {
	char buf[4096];
	if (iv->account->current)
	  {
	     Ecore_Exe *exe;
	     snprintf(buf, sizeof(buf), "%s -a %s", iv->config->image_editor, (char *) iv->account->current->file_path);
	     exe = ecore_exe_run(buf, NULL);
	     ecore_exe_free(exe);
	  }
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
   Evas_Object *o, *ic;
   char buf[4096];
   
   /*
    * XXX: configurable themes
    * */
   snprintf(buf, sizeof(buf), "%s/themes/default.edj", PACKAGE_DATA_DIR);
   elm_theme_extension_add(NULL, buf);
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
   
   o = elm_layout_add(iv->gui.win);
   elm_layout_file_set(o, buf, "iv/main");
   evas_object_size_hint_weight_set(o, 1.0, 1.0);
   elm_win_resize_object_add(iv->gui.win, o);
   evas_object_focus_set(o, 1);
   evas_object_event_callback_add(o, EVAS_CALLBACK_KEY_DOWN, on_key_down, iv);
   evas_object_show(o);
   iv->gui.ly = o;

#ifdef HAVE_ETHUMB
   edje_object_signal_callback_add(elm_layout_edje_get(o),
				   "iv,state,show_previews", "iv", on_show_previews, iv);
#endif

   o = elm_scroller_add(iv->gui.ly);
   evas_object_size_hint_weight_set(o, 1.0, 1.0);
   elm_win_resize_object_add(iv->gui.win, o);
   elm_layout_content_set(iv->gui.ly, "iv.swallow.image", o);
   iv->gui.scroller = o;

   o = elm_layout_add(iv->gui.ly);
   elm_layout_file_set(o, buf, "iv/controls");
   evas_object_size_hint_weight_set(o, 1.0, 1.0);
   elm_layout_content_set(iv->gui.ly, "iv.swallow.controls", o);
   edje_object_signal_callback_add(elm_layout_edje_get(o),
				   "iv,action,click", "iv", on_controls_click, iv);
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
   Ecore_File_Monitor *monitor;
   const char *path;

   while(iv->files)
     remove_iv_file(iv, IV_FILE(iv->files), EINA_FALSE);
   EINA_LIST_FREE(iv->dirs, path)
      eina_stringshare_del(path);
   EINA_LIST_FREE(iv->file_monitors, monitor)
      ecore_file_monitor_del(monitor);

#ifdef HAVE_ETHUMB
   if (iv->preview_files)
     eina_list_free(iv->preview_files);
#endif

   eina_stringshare_del(iv->theme_file);

   if (iv->config_edd)
     eet_data_descriptor_free(iv->config_edd);

   config_free(iv);

#ifdef HAVE_ETHUMB
   if (iv->ethumb_client)
     ethumb_client_disconnect(iv->ethumb_client);
   if (iv->gui.preview_genlist)
     elm_genlist_clear(iv->gui.preview_genlist);
   free(iv->itc);
#endif

   free(iv);
}

static void
config_init(IV *iv)
{
   Eet_Data_Descriptor_Class eddc;
   
   if (!eet_eina_file_data_descriptor_class_set(&eddc, sizeof (eddc), "IV_Config", sizeof(IV_Config)))
     {
	ERR("Unable to create the config data descriptor!");
	return iv_exit(iv);
     }

   iv->config_edd = eet_data_descriptor_file_new(&eddc);
#undef T
#undef D
#define T IV_Config 
#define D iv->config_edd
#define C_VAL(edd, type, member, dtype) EET_DATA_DESCRIPTOR_ADD_BASIC(edd, type, #member, member, dtype)
   C_VAL(D, T, config_version, EET_T_INT);
   C_VAL(D, T, img_scale, EET_T_DOUBLE);
   C_VAL(D, T, slideshow_delay, EET_T_DOUBLE);
   C_VAL(D, T, image_editor, EET_T_STRING);
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
   iv->account = calloc(1, sizeof(IV_Files_Account));

   __log_domain = eina_log_domain_register("IV", EINA_COLOR_BLUE);
   if (!__log_domain)
     {
	EINA_LOG_ERR("Could not register log domain: IV");
	elm_exit();
	return 0;
     }

   config_init(iv);

   for (i = 1; i < argc; i++)
     {
	char *real;

	if (!strncmp(argv[i], "file://", 7))
	  argv[i] = argv[i] + 7;
	if (!ecore_file_exists(argv[i]))
	  continue;

	real = ecore_file_realpath(argv[i]);
	if (ecore_file_is_dir(argv[i]))
	  iv->dirs = eina_list_append(iv->dirs,
				      eina_stringshare_add(real));
	else
	  {
	     IV_File *file = create_iv_file(iv, real);
	     iv->files = eina_inlist_append(iv->files, EINA_INLIST_GET(file));
#ifdef HAVE_ETHUMB
	     iv->preview_files = eina_list_append(iv->preview_files, file);
#endif
	  }
	free(real);
     }

   if (!iv->dirs)
     {
	if (!iv->files)
	  {
	     char *real = ecore_file_realpath(".");
	     iv->dirs = eina_list_append(iv->dirs, eina_stringshare_add(real));
	     free(real);
	  }
	else if (iv->account->count == 1)
	  {
	     IV_File *file = IV_FILE(iv->files);
	     const char *path = file->file_path;
	     char *dir = ecore_file_dir_get(path);

	     iv->dirs = eina_list_append(iv->dirs, eina_stringshare_add(dir));
	     iv->single_file = eina_stringshare_ref(path);
	     remove_iv_file(iv, file, EINA_FALSE);
	     free(dir);
	  }
	else iv->account->current = IV_FILE(iv->files);
     }
   create_main_win(iv);

#ifdef HAVE_ETHUMB
   ethumb_client_init();
   iv->connection_retry = 3;
#endif

   iv->flags.fit_changed = EINA_TRUE;
   iv->idler = ecore_idler_add(on_idler, iv);

   elm_run();

   iv_free(iv);
   eina_log_domain_unregister(__log_domain);

#ifdef HAVE_ETHUMB
   ethumb_client_shutdown();
#endif

   elm_shutdown();
   return 0; 
}

ELM_MAIN()
