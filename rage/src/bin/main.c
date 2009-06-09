#include "main.h"
#include <Ecore_Getopt.h>

typedef struct _Mode Mode;

struct _Mode
{
   int mode;
};

Evas        *evas = NULL;
char        *theme = NULL;
char        *config = NULL;
Eet_File    *eet_config = NULL;
Ecore_Timer* mouse_timeout = NULL;

static double       start_time = 0.0;
static Ecore_Evas  *ecore_evas = NULL;
static Evas_Object *o_bg       = NULL;
static Eina_List   *modes      = NULL;
static int          cmode      = NONE;

static void main_usage(void);
static int main_volume_add(void *data, int type, void *ev);
static int main_volume_del(void *data, int type, void *ev);
static void main_key_down(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void main_mouse_move(void *data, Evas *e, Evas_Object *obj, void *event_info);
static int  main_signal_exit(void *data, int ev_type, void *ev);
static void main_delete_request(Ecore_Evas *ee);
static void main_resize(Ecore_Evas *ee);
static void main_menu_config(void *data);
static void main_menu_video(void *data);
static void main_menu_dvd(void *data);
static void main_menu_audio(void *data);
static void main_menu_photo(void *data);
static void main_menu_scan(void *data);
static void main_menu_tv(void *data);
static void main_get_config(void);

static const Ecore_Getopt options = {
  "rage",
  "%prog [options]",
  "0.2.0",
  "(C) 2009 Enlightenment",
  "BSD with advertisement clause",
  "Simple yet fancy media center.",
  1,
  {
    ECORE_GETOPT_STORE_STR('e', "engine", "ecore-evas engine to use"),
    ECORE_GETOPT_CALLBACK_NOARGS
    ('E', "list-engines", "list ecore-evas engines",
     ecore_getopt_callback_ecore_evas_list_engines, NULL),
    ECORE_GETOPT_STORE_DEF_BOOL('F', "fullscreen", "fullscreen mode", 0),
    ECORE_GETOPT_CALLBACK_ARGS
    ('g', "geometry", "geometry to use in x:y:w:h form.", "X:Y:W:H",
     ecore_getopt_callback_geometry_parse, NULL),
    ECORE_GETOPT_STORE_STR
    ('t', "theme", "path to read the theme file from"),
    ECORE_GETOPT_VERSION('V', "version"),
    ECORE_GETOPT_COPYRIGHT('R', "copyright"),
    ECORE_GETOPT_LICENSE('L', "license"),
    ECORE_GETOPT_HELP('h', "help"),
    ECORE_GETOPT_SENTINEL
  }
};

int
main(int argc, char **argv)
{
   Evas_Object *o;
   int args, size;
   char *engine = NULL;
   unsigned char quit_option = 0, fullscreen = 0;
   Eina_Rectangle geometry = {0, 0, 0, 0};
   Ecore_Getopt_Value values[] = {
     ECORE_GETOPT_VALUE_STR(engine),
     ECORE_GETOPT_VALUE_BOOL(quit_option),
     ECORE_GETOPT_VALUE_BOOL(fullscreen),
     ECORE_GETOPT_VALUE_PTR_CAST(geometry),
     ECORE_GETOPT_VALUE_STR(theme),
     ECORE_GETOPT_VALUE_BOOL(quit_option),
     ECORE_GETOPT_VALUE_BOOL(quit_option),
     ECORE_GETOPT_VALUE_BOOL(quit_option),
     ECORE_GETOPT_VALUE_BOOL(quit_option),
     ECORE_GETOPT_VALUE_NONE
   };

   /* init ecore, eet, evas, edje etc. */
   start_time = ecore_time_get();
   eet_init();
   ecore_init();
   ecore_file_init();
   evas_init();
   edje_init();
   ecore_app_args_set(argc, (const char **)argv);
   /* ctrl-c / term signal - set event handler */
   ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT, main_signal_exit, NULL);
   /* try init ecore_evas - if we can't - abort */
   if (!ecore_evas_init())
     {
	printf("ERROR: cannot init ecore_evas\n");
	return -1;
     }

   main_get_config();
   //mode = *(int*)eet_read(eet_config, "/config/mode", &size);
   fullscreen = *(int*)eet_read(eet_config, "/config/fullscreen", &size);

   args = ecore_getopt_parse(&options, values, argc, argv);
   if (args < 0)
     {
        fputs("ERROR: could not parse command line options.\n", stderr);
	return -1;
     }

   if (quit_option)
     return 0;

   /* set up default theme if no custom theme is selected */
   if (!theme)
     theme = eet_read(eet_config, "/config/theme", &size);

   if (geometry.w <= 0)
     geometry.w = 1280;
   if (geometry.h <= 0)
     geometry.h = 720;

   ecore_evas = ecore_evas_new
     (engine, geometry.x, geometry.y, geometry.w, geometry.h, NULL);

   if (!ecore_evas)
     {
	fprintf(stderr, "ERROR: Cannot create canvas, engine: %s, "
		"geometry: %d,%d+%dx%d\n",
		engine ? engine : "<auto>",
		geometry.x, geometry.y, geometry.w, geometry.h);
	return -1;
     }
   ecore_evas_callback_delete_request_set(ecore_evas, main_delete_request);
   ecore_evas_callback_resize_set(ecore_evas, main_resize);
   ecore_evas_title_set(ecore_evas, "Rage");
   ecore_evas_name_class_set(ecore_evas, "main", "Rage");
   evas = ecore_evas_get(ecore_evas);
   evas_image_cache_set(evas, 8 * 1024 * 1024);
   evas_font_cache_set(evas, 1 * 1024 * 1024);
   evas_font_path_append(evas, PACKAGE_DATA_DIR"/fonts");
   /* edje animations should run at 30 fps - might make this config later */
   edje_frametime_set(1.0 / 30.0);

   /* black rectangle behind everything to catch events */
   o = evas_object_rectangle_add(evas);
   evas_object_color_set(o, 0, 0, 0, 255);
   evas_object_move(o, 0, 0);
   evas_object_resize(o, geometry.w, geometry.h);
   evas_object_show(o);
   evas_object_event_callback_add(o, EVAS_CALLBACK_KEY_DOWN, main_key_down, NULL);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_MOVE, main_mouse_move, NULL);
   evas_object_focus_set(o, 1);
   o_bg = o;

   /* if fullscreen mode - go fullscreen and hide mouse */
   if (fullscreen)
     {
	ecore_evas_fullscreen_set(ecore_evas, 1);
	ecore_evas_cursor_set(ecore_evas, "", 999, 0, 0);
     }

   /* init ui elements and volume scanner */
   if (!layout_init())
     return -1;
   background_init();
   volume_init();
   status_init();

   /* build a default menu */
   main_mode_push(MENU);
   menu_push("menu", "Main", NULL, NULL);
   menu_item_add("icon/tv", "TV",
		  "Scan all media again and update", NULL,
		  main_menu_tv, NULL, NULL, NULL, NULL);
   menu_item_add("icon/video", "Videos",
		  "Films, Movies and other video footage", NULL,
		  main_menu_video, NULL, NULL, NULL, NULL);
   menu_item_add("icon/dvd", "DVD",
		  "Play a DVD", NULL,
		  main_menu_dvd, NULL, NULL, NULL, NULL);
   menu_item_add("icon/audio", "Music",
		  "Music Albums, Songs and Tracks.", NULL,
		  main_menu_audio, NULL, NULL, NULL, NULL);
   menu_item_add("icon/photo", "Photos",
		  "Photos and images", NULL,
		  main_menu_photo, NULL, NULL, NULL, NULL);
   menu_item_add("icon/update", "Scan Media",
		  "Scan all media again and update", NULL,
		  main_menu_scan, NULL, NULL, NULL, NULL);
   menu_item_add("icon/config", "Settings",
		  "Modify settings and preferences", NULL,
		  main_menu_config, NULL, NULL, NULL, NULL);

   menu_item_enabled_set("Main", "Settings", 1);
   menu_item_enabled_set("Main", "DVD", 1);

   menu_go();

   menu_item_select("Settings");

   /* show our canvas */
   ecore_evas_show(ecore_evas);

   /* add event handlers for volume add/del event from the volume scanner
    * system so we know when volumes come and go */
   ecore_event_handler_add(VOLUME_ADD, main_volume_add, NULL);
   ecore_event_handler_add(VOLUME_DEL, main_volume_del, NULL);

   /* ... run the program core loop ... */
   ecore_main_loop_begin();

   ecore_evas_shutdown();
   ecore_file_shutdown();
   ecore_shutdown();
   eet_shutdown();
   return 0;
}

void
main_mode_push(int mode)
{
   Mode *md;

   md = calloc(1, sizeof(Mode));
   md->mode = mode;
   modes = eina_list_prepend(modes, md);
   cmode = md->mode;
}

void
main_mode_pop(void)
{
   Mode *md;

   if (!modes) return;
   md = modes->data;
   modes = eina_list_remove_list(modes, modes);
   free(md);
   if (!modes)
     {
	cmode = NONE;
     }
   else
     {
	md = modes->data;
	cmode = md->mode;
     }
}

/***/

void
main_reset(void)
{
   eet_close(eet_config);
   execlp("rage", "rage", NULL);
}

static int
main_volume_add(void *data, int type, void *ev)
{
   return 1;
}

static int
main_volume_del(void *data, int type, void *ev)
{
   return 1;
}

static void
main_key_down(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Key_Down *ev;

   ev = (Evas_Event_Key_Down *)event_info;
   if      ((!strcmp(ev->keyname, "Escape")) ||
            (!strcmp(ev->keyname, "q")) ||
            (!strcmp(ev->keyname, "Q")))
     {
	eet_close(eet_config);
	ecore_main_loop_quit();
     }
   else if (!strcmp(ev->keyname, "f"))
     {
	if (!ecore_evas_fullscreen_get(ecore_evas))
	  {
	     ecore_evas_cursor_set(ecore_evas, "", 999, 0, 0);
	     ecore_evas_fullscreen_set(ecore_evas, 1);
	  }
	else
	  {
	     ecore_evas_cursor_set(ecore_evas, NULL, 0, 0, 0);
	     ecore_evas_fullscreen_set(ecore_evas, 0);
	  }
     }
   else
     {
	switch (cmode)
	  {
	   case MENU: menu_key(ev); break;
	   case VIDEO: video_key(ev); break;
	   case DVB: dvb_key(ev); break;
	   default: break;
	  }
     }
}

static int
main_signal_exit(void *data, int ev_type, void *ev)
{
   ecore_main_loop_quit();
   return 1;
}

static void
main_delete_request(Ecore_Evas *ee)
{
   ecore_main_loop_quit();
}

static void
main_resize(Ecore_Evas *ee)
{
   Evas_Coord w, h;

   evas_output_viewport_get(evas, NULL, NULL, &w, &h);
   evas_object_resize(o_bg, w, h);
   layout_resize();
}

static void
main_menu_config(void *data)
{
   menu_push("menu", "Settings", NULL, NULL);
   menu_item_add("icon/fullscreen", "Fullscreen",
		  "Fullscreen On/Off", NULL,
		  config_option_fullscreen, ecore_evas, NULL, NULL, NULL);
   menu_item_add("icon/themes", "Themes",
		  "Select your theme", NULL,
		  config_option_themes, NULL, NULL, NULL, NULL);
   menu_item_add("icon/modes", "Modes",
		  "Change the engine Rage uses", NULL,
		  config_option_modes, ecore_evas, NULL, NULL, NULL);
   menu_item_add("icon/volumes", "Volumes",
		  "Edit your Volumes", NULL,
		  config_option_volumes, NULL, NULL, NULL, NULL);
   menu_item_enabled_set("Settings", "Fullscreen", 1);
   menu_item_enabled_set("Settings", "Themes", 1);
   menu_item_enabled_set("Settings", "Modes", 1);
   menu_item_enabled_set("Settings", "Volumes", 1);
   menu_go();
   menu_item_select("Fullscreen");
}

typedef struct _Genre          Genre;

struct _Genre
{
   const char *label;
   int count;
};

static Eina_List *
list_string_unique_append(Eina_List *list, const char *str, int count)
{
   Eina_List *l;
   Genre *ge;

   for (l = list; l; l = l->next)
     {
	ge = l->data;
	if (!strcmp(str, ge->label))
	  {
	     ge->count += count;
	     return list;
	  }
     }
   ge = calloc(1, sizeof(Genre));
   ge->label = eina_stringshare_add(str);
   ge->count = count;
   list = eina_list_append(list, ge);
   return list;
}

static void
list_string_free(Eina_List *list)
{
   while (list)
     {
	Genre *ge;

	ge = list->data;
	eina_stringshare_del(ge->label);
	free(ge);
	list = eina_list_remove_list(list, list);
     }
}

static Eina_List *
list_video_genres(void)
{
   const Eina_List *l;
   Eina_List *genres = NULL;

   /* determine toplevel genres */
   for (l = volume_items_get(); l; l = l->next)
     {
	Volume_Item *vi;

	vi = l->data;
	if (!strcmp(vi->type, "video"))
	  {
	     if (vi->genre)
	       genres = list_string_unique_append(genres, vi->genre, 1);
	  }
     }
   return genres;
}

typedef struct _Video_Lib      Video_Lib;
typedef struct _Video_Lib_Item Video_Lib_Item;

struct _Video_Lib
{
   const char *label;
   const char *path;
};

struct _Video_Lib_Item
{
   const char  *label;
   const char  *path;
   Volume_Item *vi;
};

static void
video_lib_free(void *data)
{
   Video_Lib *vl;

   vl = data;
   eina_stringshare_del(vl->label);
   eina_stringshare_del(vl->path);
   free(vl);
}

static void
video_lib_item_free(void *data)
{
   Video_Lib_Item *vli;

   vli = data;
   eina_stringshare_del(vli->label);
   eina_stringshare_del(vli->path);
   free(vli);
}

static Ecore_Timer *over_delay_timer = NULL;
static Evas_Object *over_video = NULL;

static int
main_menu_video_over_delay(void *data)
{
   Video_Lib_Item *vli;

   vli = data;
   if (over_video) minivid_del(over_video);
   over_video = minivid_add("xine", vli->vi->path, 1);
   layout_swallow("video_preview", over_video);
   over_delay_timer = NULL;
   return 0;
}

static void
main_menu_video_view(void *data)
{
   Video_Lib_Item *vli;

   vli = data;
   if (over_delay_timer)
     {
	ecore_timer_del(over_delay_timer);
	over_delay_timer = NULL;
     }
   main_mode_push(VIDEO);
   if (over_video)
     {
	minivid_del(over_video);
	over_video = NULL;
     }
   video_init("xine", vli->vi->path, "video");
}

static void
main_menu_video_over(void *data)
{
   Video_Lib_Item *vli;

   vli = data;
   if (over_delay_timer) ecore_timer_del(over_delay_timer);
   over_delay_timer = ecore_timer_add(0.5, main_menu_video_over_delay, vli);
}

static void
main_menu_video_out(void *data)
{
   Video_Lib_Item *vli;

   vli = data;
   if (over_delay_timer)
     {
	ecore_timer_del(over_delay_timer);
	over_delay_timer = NULL;
     }
   if (over_video)
     {
	minivid_del(over_video);
	over_video = NULL;
     }
}

static void
main_menu_video_library(void *data)
{
   const Eina_List *l;
   Eina_List *genres = NULL, *glist = NULL;
   Video_Lib *vl;
   Video_Lib_Item *vli;

   vli = data;
   vl = (Video_Lib *)menu_data_get();
   if (!vl)
     {
	vl = calloc(1, sizeof(Video_Lib));
	vl->label = eina_stringshare_add("Library");
	vl->path = eina_stringshare_add("");
	menu_push("menu", vl->label, video_lib_free, vl);
     }
   else
     {
	vl = calloc(1, sizeof(Video_Lib));
	vl->label = eina_stringshare_add(ecore_file_file_get(vli->path));
	vl->path = eina_stringshare_add(vli->path);
	menu_push("menu", vl->label, video_lib_free, vl);
     }

   /* determine toplevel genres */
   genres = list_video_genres();
   if (genres)
     {
	int vlpn;

	vlpn = strlen(vl->path);
	printf("--- %s\n", vl->path);
	for (l = genres; l; l = l->next)
	  {
	     Genre *ge;

	     ge = l->data;
	     if (vlpn > 0)
	       {
		  if ((!strncmp(vl->path, ge->label, vlpn)) &&
		      (strlen(ge->label) != vlpn))
		    {
		       char *s, *p;

		       s = strdup(ge->label + vlpn + 1);
		       p = strchr(s, '/');
		       if (p) *p = 0;
		       printf("APP %s %i\n", s, ge->count);
		       glist = list_string_unique_append(glist, s, ge->count);
		       free(s);
		    }
	       }
	     else
	       {
		  char *s, *p;

		  s = strdup(ge->label);
		  p = strchr(s, '/');
		  if (p) *p = 0;
		  printf("APP2 %s %i\n", s, ge->count);
		  glist = list_string_unique_append(glist, s, ge->count);
		  free(s);
	       }
	  }
	list_string_free(genres);
	if (glist)
	  {
	     for (l = glist; l; l = l->next)
	       {
		  Genre *ge;
		  char buf[4096];

		  ge = l->data;
		  if (vl->path[0])
		    snprintf(buf, sizeof(buf), "%s/%s", vl->path, ge->label);
		  else
		    snprintf(buf, sizeof(buf), "%s", ge->label);
		  vli = calloc(1, sizeof(Video_Lib_Item));
		  vli->label = eina_stringshare_add(ge->label);
		  vli->path = eina_stringshare_add(buf);
		  snprintf(buf, sizeof(buf), "%i", ge->count);
		  menu_item_add("icon/genre", vli->label,
				"", buf,
				main_menu_video_library, vli,
				video_lib_item_free,
				NULL, NULL);
		  menu_item_enabled_set(vl->label, vli->label, 1);
	       }
	     menu_go();
	     menu_item_select(((Genre *)(glist->data))->label);
	     list_string_free(glist);
	  }
	else
	  {
	     const char *sel = NULL;

	     for (l = volume_items_get(); l; l = l->next)
	       {
		  Volume_Item *vi;

		  vi = l->data;
		  if (!strcmp(vi->type, "video"))
		    {
		       if (!strcmp(vi->genre, vl->path))
			 {
			    char buf[4096];

			    buf[0] = 0;
			    vli = calloc(1, sizeof(Video_Lib_Item));
			    vli->label = eina_stringshare_add(vi->name);
			    vli->path = eina_stringshare_add(vi->rpath);
			    vli->vi = vi;
//			    snprintf(buf, sizeof(buf), "3:00:00");
			    menu_item_add(vli->path, vli->label,
					  "", buf,
					  main_menu_video_view, vli,
					  video_lib_item_free,
					  main_menu_video_over,
					  main_menu_video_out);
			    menu_item_enabled_set(vl->label, vli->label, 1);
			    if (!sel) sel = vli->label;
			 }
		    }
	       }
	     menu_go();
	     if (sel) menu_item_select(sel);
	  }
     }
}

static void
main_menu_dvd_watch(void *data)
{
   Video_Lib_Item *vli;

   vli = data;
   if (over_delay_timer)
     {
	ecore_timer_del(over_delay_timer);
	over_delay_timer = NULL;
     }
   main_mode_push(VIDEO);
   if (over_video)
     {
	minivid_del(over_video);
	over_video = NULL;
     }
   video_init("xine", "dvd://", "video");
}

static void
main_menu_video(void *data)
{
   menu_push("menu", "Video", NULL, NULL);
   menu_item_add("icon/resume", "Resume",
		  "Resume last Video played", NULL,
		  NULL, NULL, NULL, NULL, NULL);
   menu_item_add("icon/favorites", "Favorites",
		  "Favorite Videos", NULL,
		  NULL, NULL, NULL, NULL, NULL);
   menu_item_add("icon/book", "Library",
		  "Browse all of your Videos", NULL,
		  main_menu_video_library, NULL, NULL, NULL, NULL);
   menu_item_enabled_set("Video", "Resume", 0);
   menu_item_enabled_set("Video", "Favorites", 0);
   menu_item_enabled_set("Video", "Library", 1);

   menu_go();
   menu_item_select("Library");
}

static void
main_menu_dvd(void *data)
{
   menu_push("menu", "DVD", NULL, NULL);
   menu_item_add("icon/resume", "Resume",
		 "Resume DVD from last play", NULL,
		 NULL, NULL, NULL, NULL, NULL);
   menu_item_add("icon/dvd", "Rip",
		 "Rip DVD to Disk for storage", NULL,
		 NULL, NULL, NULL, NULL, NULL);
   menu_item_add("icon/dvd", "Watch",
		 "Watch your DVD", NULL,
		 main_menu_dvd_watch, NULL, NULL, NULL, NULL);
   menu_item_enabled_set("DVD", "Resume", 0);
   menu_item_enabled_set("DVD", "Rip", 0);
   menu_item_enabled_set("DVD", "Watch", 1);
   menu_go();
   menu_item_select("Watch");
}

static void
main_menu_audio(void *data)
{
   menu_push("menu", "Audio", NULL, NULL);
   menu_item_add("icon/bug", "Disabled",
		  "This feature is incomplete", NULL,
		  NULL, NULL, NULL, NULL, NULL);
   menu_go();
   menu_item_select("Disabled");
}

static void
main_menu_photo(void *data)
{
   menu_push("menu", "Photo", NULL, NULL);
   menu_item_add("icon/bug", "Disabled",
		  "This feature is incomplete", NULL,
		  NULL, NULL, NULL, NULL, NULL);
   menu_go();
   menu_item_select("Disabled");
}

static void
main_menu_scan(void *data)
{
   volume_update();
}

static void
main_menu_tv(void *data)
{
   if (over_delay_timer)
     {
	ecore_timer_del(over_delay_timer);
	over_delay_timer = NULL;
     }
   main_mode_push(DVB);
   if (over_video)
     {
	minivid_del(over_video);
	over_video = NULL;
     }
   dvb_init("xine", "", "video");
//   system("tvtime -m -n PAL -f custom");
/*
   system("xine -f --no-gui "
          "dvb://0 "
          "dvb://1 "
          "dvb://2 "
          "dvb://3 "
          "dvb://4 "
          "dvb://5 "
          "dvb://6 "
          "dvb://7 "
          "dvb://8 "
          "dvb://9 "
          "dvb://10 "
          "dvb://11 "
          "dvb://12 "
          "dvb://13 "
          "dvb://14 "
          "dvb://15 "
          "dvb://16 "
          "dvb://17 "
          "dvb://18 "
          "dvb://19 "
          "dvb://20 "
          "dvb://21 "
          "dvb://22 "
          "dvb://23 "
          );
 */
}

int
main_mouse_timeout(void* data)
{
   ecore_evas_cursor_set(ecore_evas, "", 999, 0, 0);
   mouse_timeout = NULL;
   return ECORE_CALLBACK_CANCEL;
}

static void
main_mouse_move(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   if (mouse_timeout)
      ecore_timer_delay(mouse_timeout, 1-ecore_timer_pending_get(mouse_timeout));
   else
   {
      mouse_timeout = ecore_timer_add(1, main_mouse_timeout, NULL);
      ecore_evas_cursor_set(ecore_evas, NULL, 0, 0, 0);
   }
}

static void
main_get_config(void)
{
   /* load config */
   char buf[4096];
   int i = 0;

   if (getenv("HOME"))
      snprintf(buf, sizeof(buf), "%s/.rage", getenv("HOME"));
   else if (getenv("TMPDIR"))
      snprintf(buf, sizeof(buf), "%s/.rage", getenv("TMPDIR"));
   else
      snprintf(buf, sizeof(buf), "%s/.rage", "/tmp");
   config = strdup(buf);

   snprintf(buf, sizeof(buf), "%s/config.eet", config);

   if (!ecore_file_exists(buf))
   {
      if (!ecore_file_is_dir(config))
	 ecore_file_mkpath(config);

      eet_config = eet_open(buf, EET_FILE_MODE_WRITE);

      /* write default config */
      eet_write(eet_config, "/config/fullscreen", &i, sizeof(int), 0);
      eet_write(eet_config, "/config/theme", PACKAGE_DATA_DIR"/default.edj",
	    	sizeof(PACKAGE_DATA_DIR"/default.edj"), 0);
      eet_write(eet_config, "/config/mode", &i, sizeof(int), 0);
      eet_close(eet_config);
   }

   eet_config = eet_open(buf, EET_FILE_MODE_READ_WRITE);
}
