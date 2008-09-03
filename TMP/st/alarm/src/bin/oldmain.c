#include "main.h"

typedef struct _Mode Mode;

struct _Mode
{
   int mode;
};

Evas        *evas = NULL;
char        *theme = NULL;

static double       start_time = 0.0;
static Ecore_Evas  *ecore_evas = NULL;
static int          startw     = 480;
static int          starth     = 640;
static int          img_cache  = 8 * 1024;
static int          fnt_cache  = 1 * 1024;
static Evas_Object *o_bg       = NULL;
static Evas_Object *o_wp       = NULL;

static void main_usage(void);
static void main_key_down(void *data, Evas *e, Evas_Object *obj, void *event_info);
static int  main_signal_exit(void *data, int ev_type, void *ev);
static void main_delete_request(Ecore_Evas *ee);
static void main_resize(Ecore_Evas *ee);

/* tests */
//#define IMGFLIP 1
#define KBDPROTO 1

static void test_init(void);

static void oo_init(void);

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

	     snprintf(buf, sizeof(buf), "%s/themes/%s.edj", PACKAGE_DATA_DIR, argv[i +1]);
	     theme = strdup(buf);
	     i++;
	  }
	else if (!strcmp(argv[i], "-ic"))
	  {
	     img_cache = atoi(argv[i + 1]);
	     i++;
	  }
	else if (!strcmp(argv[i], "-fc"))
	  {
	     fnt_cache = atoi(argv[i + 1]);
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
	else if (!strcmp(argv[i], "-x11-16"))
	  mode = 4;
	else if (!strcmp(argv[i], "-fs"))
	  fullscreen = 1;
	else
	  main_usage();
     }
   
   /* set up default theme if no custom theme is selected */
   if (!theme)
     theme = strdup(PACKAGE_DATA_DIR"/themes/default.edj");
   /* create the canvas based on engine mode */
   if (mode == 0)
     ecore_evas = ecore_evas_software_x11_new(NULL, 0, 0, 0, startw, starth);
   if (mode == 1)
     ecore_evas = ecore_evas_gl_x11_new(NULL, 0, 0, 0, startw, starth);
   if (mode == 2)
     ecore_evas = ecore_evas_fb_new(NULL, 0, startw, starth);
   if (mode == 3)
     ecore_evas = ecore_evas_xrender_x11_new(NULL, 0, 0, 0, startw, starth);
   if (mode == 4)
     ecore_evas = ecore_evas_software_x11_16_new(NULL, 0, 0, 0, startw, starth);
   if (!ecore_evas)
     {
	printf("ERROR: Cannot create canvas\n");
	return -1;
     }
   ecore_evas_callback_delete_request_set(ecore_evas, main_delete_request);
   ecore_evas_callback_resize_set(ecore_evas, main_resize);
   ecore_evas_title_set(ecore_evas, "Ello");
   ecore_evas_name_class_set(ecore_evas, "main", "Ello");
   evas = ecore_evas_get(ecore_evas);
   evas_image_cache_set(evas, img_cache * 1024);
   evas_font_cache_set(evas, fnt_cache * 1024);
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

   o = edje_object_add(evas);
   edje_object_file_set(o, theme, "background");
   evas_object_move(o, 0, 0);
   evas_object_resize(o, startw, starth);
   evas_object_show(o);
   o_wp = o;
   
   /* if fullscreen mode - go fullscreen and hide mouse */
   if (fullscreen)
     {
	ecore_evas_fullscreen_set(ecore_evas, 1);
	ecore_evas_cursor_set(ecore_evas, "", 999, 0, 0);
     }

   test_init();
   oo_init();
   
   /* show our canvas */
   ecore_evas_show(ecore_evas);

   /* ... run the program core loop ... */
   ecore_main_loop_begin();

   edje_shutdown();
   ecore_evas_shutdown();
   evas_shutdown();
   ecore_file_shutdown();
   ecore_shutdown();
   eet_shutdown();
   return 0;
}

/***/

static void
main_usage(void)
{
   printf("Usage:\n");
   printf("  ello [0 or more options below ...]\n"
	  "    [-x11] [-gl] [-fb] [-xr] [-x11-16] [-g WxH] [-fs] [-t theme]\n"
	  "    [-ic img_cache] [-fc fnt_cache]\n"
	  );
   exit(-1);
}

static void
main_fullscreen_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   ecore_main_loop_quit();
}

static void
main_fullscreen(int on)
{
   static Evas_Object *o;
   
   if (on)
     {
	ecore_evas_cursor_set(ecore_evas, "", 999, 0, 0);
	ecore_evas_fullscreen_set(ecore_evas, 1);
	
	o = evas_object_text_add(evas);
	evas_object_text_font_set(o, "Sans", 30);
	evas_object_text_text_set(o, "X");
	evas_object_move(o, 0, 0);
	evas_object_color_set(o, 0, 0, 0, 255);
	evas_object_show(o); 
	
	evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_DOWN, main_fullscreen_mouse_down, NULL);
     }
   else
     {
	evas_object_del(o);
	o = NULL;
	ecore_evas_cursor_set(ecore_evas, NULL, 0, 0, 0);
	ecore_evas_fullscreen_set(ecore_evas, 0);
     }
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
	  main_fullscreen(1);
	else
	  main_fullscreen(0);
     }
   else
     {
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
   evas_object_resize(o_wp, w, h);
}

static Evas_Object *oo = NULL;
static Evas_Object *ooe = NULL;
static void
_oo_cb_mouse_down(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Down *ev;
   
   ev = event_info;
}
static void
_oo_cb_mouse_up(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Up *ev;
   
   ev = event_info;
}
static void
_oo_cb_mouse_move(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Move *ev;
   Evas_Coord x, y;

   ev = event_info;
   if (!ev->buttons) return;
   evas_object_geometry_get(data, &x, &y, NULL, NULL);
   evas_object_move(data, 
		    x + (ev->cur.output.x - ev->prev.output.x),
		    y + (ev->cur.output.y - ev->prev.output.y));
   evas_object_move(obj, 
		    x + (ev->cur.output.x - ev->prev.output.x),
		    y + (ev->cur.output.y - ev->prev.output.y));
}
static void
oo_init(void)
{
   char buf[4096];
   
   oo = edje_object_add(evas);
   snprintf(buf, sizeof(buf), "%s/objects/clock.edj", PACKAGE_DATA_DIR);
   edje_object_file_set(oo, buf, "object");
   evas_object_move(oo, 0, 0);
   evas_object_resize(oo, 448, 448);
   evas_object_show(oo);

   ooe = evas_object_rectangle_add(evas);
   evas_object_color_set(ooe, 0, 0, 0, 0);
   evas_object_move(ooe, 0, 0);
   evas_object_resize(ooe, 448, 448);
   evas_object_show(ooe);
   
   evas_object_event_callback_add(ooe, EVAS_CALLBACK_MOUSE_DOWN, _oo_cb_mouse_down, oo);
   evas_object_event_callback_add(ooe, EVAS_CALLBACK_MOUSE_UP, _oo_cb_mouse_up, oo);
   evas_object_event_callback_add(ooe, EVAS_CALLBACK_MOUSE_MOVE, _oo_cb_mouse_move, oo);
}

#if KBDPROTO
//static Ecore_X_Atom kb_state, kb_on, kb_off, kb_alpha, kb_numeric;
static int mode = 0;
static Evas_Object *bt = NULL;
static void mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   mode++; if (mode > 3) mode = 0;
   
   if (mode == 0)
     {
	evas_object_text_text_set(bt, "OFF");
	ecore_x_e_virtual_keyboard_state_set(ecore_evas_software_x11_window_get(ecore_evas),
					     ECORE_X_VIRTUAL_KEYBOARD_STATE_OFF);
//	ecore_x_window_prop_atom_set(ecore_evas_software_x11_window_get(ecore_evas),
//				     kb_state, &kb_off, 1);
     }
   if (mode == 1)
     {
	evas_object_text_text_set(bt, "ON");
	ecore_x_e_virtual_keyboard_state_set(ecore_evas_software_x11_window_get(ecore_evas),
					     ECORE_X_VIRTUAL_KEYBOARD_STATE_ON);
//	ecore_x_window_prop_atom_set(ecore_evas_software_x11_window_get(ecore_evas),
//				     kb_state, &kb_on, 1);
     }
   if (mode == 2)
     {
	evas_object_text_text_set(bt, "ALPHA");
	ecore_x_e_virtual_keyboard_state_set(ecore_evas_software_x11_window_get(ecore_evas),
					     ECORE_X_VIRTUAL_KEYBOARD_STATE_ALPHA);
//	ecore_x_window_prop_atom_set(ecore_evas_software_x11_window_get(ecore_evas),
//				     kb_state, &kb_alpha, 1);
     }
   if (mode == 3)
     {
	evas_object_text_text_set(bt, "NUMERIC");
	ecore_x_e_virtual_keyboard_state_set(ecore_evas_software_x11_window_get(ecore_evas),
					     ECORE_X_VIRTUAL_KEYBOARD_STATE_NUMERIC);
//	ecore_x_window_prop_atom_set(ecore_evas_software_x11_window_get(ecore_evas),
//				     kb_state, &kb_numeric, 1);
     }
}
static void
test_init(void)
{
   Evas_Object *o;

//   kb_state = ecore_x_atom_get("_E_VIRTUAL_KEYBOARD_STATE");
//   kb_on = ecore_x_atom_get("_E_VIRTUAL_KEYBOARD_ON");
//   kb_off = ecore_x_atom_get("_E_VIRTUAL_KEYBOARD_OFF");
//   kb_alpha = ecore_x_atom_get("_E_VIRTUAL_KEYBOARD_ALPHA");
//   kb_numeric = ecore_x_atom_get("_E_VIRTUAL_KEYBOARD_NUMERIC");
   
   mode = 0;
   ecore_x_e_virtual_keyboard_state_set(ecore_evas_software_x11_window_get(ecore_evas),
					ECORE_X_VIRTUAL_KEYBOARD_STATE_OFF);
//   ecore_x_window_prop_atom_set(ecore_evas_software_x11_window_get(ecore_evas),
//				kb_state, &kb_off, 1);

   o = evas_object_text_add(evas);
   evas_object_text_font_set(o, "Sans", 20);
   evas_object_text_text_set(o, "OFF");
   evas_object_move(o, 200, 200);
   evas_object_color_set(o, 0, 0, 0, 255);
   evas_object_show(o);
   bt = o;

   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_DOWN, mouse_down, NULL);
}
#elif IMGFLIP
static Evas_Object *tsto;
static void
test_frame(void)
{
   const char *images[] =
     {
	"im0.jpg",
	  "im1.jpg",
	  "im2.jpg",
	  "im3.jpg",
	  "im4.jpg",
	  "im5.jpg",
	  "im6.jpg",
	  "im7.jpg",
	  "im8.jpg",
	  "im9.jpg"
     };
   char buf[4096];
   static int pos = 0;
   static double start_time = 0.0;
   
   if ((pos == 0) && (start_time > 0.0))
     {
	double t, tdif;
	
	t = ecore_time_get();
	tdif = t - start_time;
	start_time = t;
	printf("FPS: %3.3f (10.0 / %3.3f)\n", 10.0 / tdif, tdif);
     }
   else if (start_time == 0.0)
     start_time = ecore_time_get();
   
   snprintf(buf, sizeof(buf), "%s/images/%s", PACKAGE_DATA_DIR, images[pos]);
   evas_object_image_file_set(tsto, buf, NULL);
   
   pos++;
   if (pos >= 10) pos = 0;
}
static int
test_timer(void *data)
{
   return 0;
}
static int
test_idler_enterer(void *data)
{
   test_frame();
   ecore_timer_add(0.0, test_timer, NULL);
   return 1;
}
static void
test_init(void)
{
   tsto = evas_object_image_add(evas);
   evas_object_move(tsto, 0, 0);
   evas_object_resize(tsto, 480, 640);
   evas_object_image_fill_set(tsto, 0, 0, 480, 640);
   evas_object_show(tsto);
   ecore_idle_enterer_add(test_idler_enterer, NULL);
}
#else
static void test_init(void){}
#endif
