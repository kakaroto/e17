#include "main.h"

typedef struct _Mode Mode;

struct _Mode
{
   int mode;
};

Evas        *evas = NULL;
char        *theme = NULL;
char        *config = NULL;

static double       start_time = 0.0;
static Ecore_Evas  *ecore_evas = NULL;
static int          startw     = 1280;
static int          starth     = 720;
static Evas_Object *o_bg       = NULL;
static Evas_List   *modes      = NULL;
static int          cmode      = NONE;

static void main_usage(void);
static int main_volume_add(void *data, int type, void *ev);
static int main_volume_del(void *data, int type, void *ev);
static void main_key_down(void *data, Evas *e, Evas_Object *obj, void *event_info);
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

int
main(int argc, char **argv)
{
   Evas_Object *o;
   int mode = 0, fullscreen = 0;
   int i;

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

   /* parse cmd-line options */
   for (i = 1; i < argc; i++)
     {
	if (!strcmp(argv[i], "-g"))
	  {
	     int n, w, h;
	     char buf[16], buf2[16];

	     n = sscanf(argv[i +1], "%10[^x]x%10s", buf, buf2);
	     if (n == 2)
	       {
		  w = atoi(buf);
		  h = atoi(buf2);
		  startw = w;
		  starth = h;
	       }
	     i++;
	  }
	else if (!strcmp(argv[i], "-t"))
	  {
	     char buf[4096];

	     snprintf(buf, sizeof(buf), "%s/%s.edj", PACKAGE_DATA_DIR, argv[i +1]);
	     theme = strdup(buf);
	     i++;
	  }
	else if (!strcmp(argv[i], "-cf"))
	  {
	     config = strdup(argv[i + 1]);
	     i++;
	  }
	else if (!strcmp(argv[i], "-x11"))
	  mode = 0;
	else if (!strcmp(argv[i], "-gl"))
	  mode = 1;
	else if (!strcmp(argv[i], "-fb"))
	  mode = 2;
	else if (!strcmp(argv[i], "-xr"))
	  mode = 3;
        else if (!strcmp(argv[1], "-dfb"))
	  mode = 4;
	else if (!strcmp(argv[1], "-sdl"))
	  mode = 5;
	else if (!strcmp(argv[i], "-fs"))
	  fullscreen = 1;
	else
	  main_usage();
     }

   /* load config */
   if (!config)
     {
	char buf[4096];

	if (getenv("HOME"))
	  snprintf(buf, sizeof(buf), "%s/.rage", getenv("HOME"));
	else if (getenv("TMPDIR"))
	  snprintf(buf, sizeof(buf), "%s/.rage", getenv("TMPDIR"));
	else
	  snprintf(buf, sizeof(buf), "%s/.rage", "/tmp");
	config = strdup(buf);
     }
   if (!ecore_file_is_dir(config)) ecore_file_mkpath(config);

   /* set up default theme if no custom theme is selected */
   if (!theme)
     theme = strdup(PACKAGE_DATA_DIR"/default.edj");
   /* create the canvas based on engine mode */
   if (mode == 0)
     ecore_evas = ecore_evas_software_x11_new(NULL, 0,  0, 0, startw, starth);
   if (mode == 1)
     ecore_evas = ecore_evas_gl_x11_new(NULL, 0, 0, 0, startw, starth);
   if (mode == 2)
     ecore_evas = ecore_evas_fb_new(NULL, 0, startw, starth);
   if (mode == 3)
     ecore_evas = ecore_evas_xrender_x11_new(NULL, 0, 0, 0, startw, starth);
   if (mode == 4)
     ecore_evas = ecore_evas_directfb_new(NULL, 0, 0, 0, startw, starth);
   if (mode == 5)
     ecore_evas = ecore_evas_sdl_new(NULL, startw, starth, 0, 1, 1, 0);
   if (!ecore_evas)
     {
	printf("ERROR: Cannot create canvas\n");
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
   evas_object_resize(o, startw, starth);
   evas_object_show(o);
   evas_object_event_callback_add(o, EVAS_CALLBACK_KEY_DOWN, main_key_down, NULL);
   evas_object_focus_set(o, 1);
   o_bg = o;

   /* if fullscreen mode - go fullscreen and hide mouse */
   if (fullscreen)
     {
	ecore_evas_fullscreen_set(ecore_evas, 1);
	ecore_evas_cursor_set(ecore_evas, "", 999, 0, 0);
     }

   /* init ui elements and volume scanner */
   layout_init();
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
   modes = evas_list_prepend(modes, md);
   cmode = md->mode;
}

void
main_mode_pop(void)
{
   Mode *md;

   if (!modes) return;
   md = modes->data;
   modes = evas_list_remove_list(modes, modes);
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

static void
main_usage(void)
{
   printf("Usage:\n");
   printf("  rage "
	  "[-x11] [-gl] [-fb] [-dfb] [-sdl] [-xr] [-g WxH] [-fs] "
	  "[-t theme] [-cf dir]\n"
	  );
   exit(-1);
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
   if      (!strcmp(ev->keyname, "Escape"))
     ecore_main_loop_quit();
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
   menu_item_add("icon/config", "Option 1",
		  "Option 1", NULL,
		  NULL, NULL, NULL, NULL, NULL);
   menu_item_add("icon/config", "Option 2",
		  "Option 2", NULL,
		  NULL, NULL, NULL, NULL, NULL);
   menu_item_add("icon/config", "Option 3",
		  "Option 3", NULL,
		  NULL, NULL, NULL, NULL, NULL);
   menu_item_add("icon/config", "Option 4",
		  "Option 4", NULL,
		  NULL, NULL, NULL, NULL, NULL);
   menu_item_enabled_set("Settings", "Option 1", 1);
   menu_item_enabled_set("Settings", "Option 2", 1);
   menu_item_enabled_set("Settings", "Option 3", 1);
   menu_item_enabled_set("Settings", "Option 4", 1);
   menu_go();
   menu_item_select("Option 1");
}

typedef struct _Genre          Genre;

struct _Genre
{
   const char *label;
   int count;
};

static Evas_List *
list_string_unique_append(Evas_List *list, const char *str, int count)
{
   Evas_List *l;
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
   ge->label = evas_stringshare_add(str);
   ge->count = count;
   list = evas_list_append(list, ge);
   return list;
}

static void
list_string_free(Evas_List *list)
{
   while (list)
     {
	Genre *ge;

	ge = list->data;
	evas_stringshare_del(ge->label);
	free(ge);
	list = evas_list_remove_list(list, list);
     }
}

static Evas_List *
list_video_genres(void)
{
   const Evas_List *l;
   Evas_List *genres = NULL;

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
   evas_stringshare_del(vl->label);
   evas_stringshare_del(vl->path);
   free(vl);
}

static void
video_lib_item_free(void *data)
{
   Video_Lib_Item *vli;

   vli = data;
   evas_stringshare_del(vli->label);
   evas_stringshare_del(vli->path);
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
   const Evas_List *l;
   Evas_List *genres = NULL, *glist = NULL;
   Video_Lib *vl;
   Video_Lib_Item *vli;

   vli = data;
   vl = (Video_Lib *)menu_data_get();
   if (!vl)
     {
	vl = calloc(1, sizeof(Video_Lib));
	vl->label = evas_stringshare_add("Library");
	vl->path = evas_stringshare_add("");
	menu_push("menu", vl->label, video_lib_free, vl);
     }
   else
     {
	vl = calloc(1, sizeof(Video_Lib));
	vl->label = evas_stringshare_add(ecore_file_file_get(vli->path));
	vl->path = evas_stringshare_add(vli->path);
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
		  vli->label = evas_stringshare_add(ge->label);
		  vli->path = evas_stringshare_add(buf);
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
			    vli->label = evas_stringshare_add(vi->name);
			    vli->path = evas_stringshare_add(vi->rpath);
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
   system("tvtime -m -n PAL -f custom");
}
