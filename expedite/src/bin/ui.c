#include "main.h"

#define PROTO
#include "tests.h"
#undef PROTO

static double start = 0.0;

static double t_start = 0.0;
static double t_loop = 0.0;
static int    f_start = 0;
static int    f_loop = 0;
static char   data_dir[4096];

typedef struct _Menu_Item Menu_Item;

struct _Menu_Item
{
   Evas_Object *o_icon;
   char *icon;
   char *text;
   void (*func) (void);
};

static Evas_Object *o_bg = NULL;
static Evas_Object *o_wallpaper = NULL;
static Evas_Object *o_title = NULL;
static Evas_Object *o_byline = NULL;
static Evas_Object *o_menu_logo = NULL;
static Evas_Object *o_menu_icon = NULL;
static Evas_Object *o_menu_icon_sel = NULL;
static Evas_Object *o_menu_icon_sel2 = NULL;
static Evas_Object *o_menu_text_sel = NULL;
static Evas_Object *o_menu_title = NULL;
static Evas_Object *o_menu_title2 = NULL;
static Eina_List   *menu = NULL;
static int          menu_sel = 0;
static int          menu_active = 0;
static double       menu_anim = 0.0;
static double       menu_anim_sel = 0.0;

static double       p_fps = 0.0;

static void (*key_func) (char *key) = NULL;
static void (*loop_func) (double t, int f) = NULL;

static int run_all = 0;
static int run_test = 0;
static int list_test = 0;
static int exit_after_test = 0;

static void
_ui_exit(void)
{
   while (menu)
     {
        Menu_Item *mi;

        mi = (Menu_Item *)menu->data;
        free(mi->icon);
        free(mi->text);
        free(mi);
        menu = eina_list_remove_list(menu, menu);
     }
   engine_abort();
}

static void
_ui_all(void)
{
   Eina_List *l;
   double fps = 0.0;
   int t_count = 0;

   evas_object_hide(o_menu_logo);
   evas_object_hide(o_menu_title);
   evas_object_hide(o_menu_title2);
   evas_object_hide(o_menu_icon);
   evas_object_hide(o_menu_icon_sel);
   evas_object_hide(o_menu_icon_sel2);
   evas_object_hide(o_menu_text_sel);
   for (l = menu; l; l = l->next)
     {
	Menu_Item *mi;

	mi = l->data;
	if ((mi->func == about_start) ||
	    (mi->func == _ui_exit) ||
	    (mi->func == _ui_all))
	  continue;
	if (mi->func) mi->func();
	while (p_fps == 0.0)
	  {
	     ui_loop();
	     engine_loop();
	     evas_render(evas);
	  }
	/* This give time to delete the objects of the previous test and make
	   the result of next test more accurate. Draw back, some time is not
	   counted at all. */
	evas_render(evas);
	t_count++;
	fps += p_fps;
	key_func("Escape");
     }
   if (t_count > 0) printf("%4.2f , EVAS SPEED\n", fps / t_count);
}


static void
_ui_num(int n)
{
   Eina_List *l;
   double fps = 0.0;
   int t_count = 0;
   Menu_Item *mi;

   evas_object_hide(o_menu_logo);
   evas_object_hide(o_menu_title);
   evas_object_hide(o_menu_title2);
   evas_object_hide(o_menu_icon);
   evas_object_hide(o_menu_icon_sel);
   evas_object_hide(o_menu_icon_sel2);
   evas_object_hide(o_menu_text_sel);
   mi = eina_list_nth(menu, n);
   if (mi)
     {
	if ((mi->func == about_start) ||
	    (mi->func == _ui_exit) ||
	    (mi->func == _ui_all))
	  goto done;
	if (mi->func) mi->func();
	while (p_fps == 0.0)
	  {
	     ui_loop();
	     engine_loop();
	     evas_render(evas);
	  }
	/* This give time to delete the objects of the previous test and make
	   the result of next test more accurate. Draw back, some time is not
	   counted at all. */
	evas_render(evas);
	t_count++;
	fps += p_fps;
	key_func("Escape");
     }
   done:
   if (t_count > 0) printf("%4.2f , EVAS SPEED\n", fps / t_count);
}

static void
_ui_select(void)
{
   Eina_List *l;
   int i;
   void (*func) (void) = NULL;

   evas_object_hide(o_menu_logo);
   evas_object_hide(o_menu_title);
   evas_object_hide(o_menu_title2);
   evas_object_hide(o_menu_icon);
   evas_object_hide(o_menu_icon_sel);
   evas_object_hide(o_menu_icon_sel2);
   evas_object_hide(o_menu_text_sel);
   for (i = 0, l = menu; l; l = l->next, i++)
     {
	Menu_Item *mi;

	mi = l->data;
	evas_object_hide(mi->o_icon);
	if (i == menu_sel)
	  func = mi->func;
     }
   menu_active = 0;
   if (func) func();
}

static void
_ui_key(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Key_Down *ev;

   ev = event_info;
   if (key_func)
     {
	key_func(ev->keyname);
	return;
     }
   if ((!strcmp(ev->keyname, "Escape")) ||
       (!strcmp(ev->keyname, "q")) ||
       (!strcmp(ev->keyname, "Q")))
     {
	engine_abort();
     }
   if (menu_active)
     {
	if (!strcmp(ev->keyname, "Left")) menu_sel++;
	if (!strcmp(ev->keyname, "Right")) menu_sel--;
	if (menu_sel < 0) menu_sel = 0;
	else if (menu_sel >= eina_list_count(menu)) menu_sel = eina_list_count(menu) - 1;
	menu_anim_sel = menu_sel;
	if (!strcmp(ev->keyname, "Return")) _ui_select();
     }
}

static Evas_Coord down_x, down_y;
static int down = 0;
static int down_menu_sel = 0;

static void
_ui_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Down *ev;

   ev = event_info;
   if (ev->button != 1) return;
   if (menu_active)
     {
	down_x = ev->canvas.x;
	down_y = ev->canvas.y;
	down++;
	down_menu_sel = menu_sel;
     }
   else
     {
     }
}

static void
_ui_mouse_up(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Up *ev;

   ev = event_info;
   if (ev->button != 1) return;
   if (menu_active)
     {
	Evas_Coord dx, dy;

	dx = ev->canvas.x - down_x;
	dy = ev->canvas.y - down_y;
	if ((((dx * dx) + (dy * dy)) < (20 * 20)) &&
	    (menu_sel == down_menu_sel))
	  _ui_select();
	down--;
     }
   else
     {
	evas_event_feed_key_down(evas, "Escape", "Escape", NULL, NULL, 0, NULL);
	evas_event_feed_key_up(evas, "Escape", "Escape", NULL, NULL, 0, NULL);
     }
}

static void
_ui_mouse_move(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Move *ev;

   ev = event_info;
   if (!down) return;
   if (menu_active)
     {
	menu_sel = down_menu_sel + ((ev->cur.canvas.x - down_x) / 25);
	/* scroll */
	if (menu_sel < 0) menu_sel = 0;
	else if (menu_sel >= eina_list_count(menu)) menu_sel = eina_list_count(menu) - 1;
	menu_anim_sel = menu_sel;
     }
   else
     {
     }
}

static void
_ui_menu_item_add(char *icon, char *text, void (*func) (void))
{
   Menu_Item *mi;

   mi = malloc(sizeof(Menu_Item));
   mi->o_icon = evas_object_image_add(evas);
   evas_object_image_file_set(mi->o_icon, build_path(icon), NULL);
   evas_object_resize(mi->o_icon, 32, 32);
   evas_object_image_fill_set(mi->o_icon, 0, 0, 32, 32);
   mi->icon = strdup(icon);
   mi->text = strdup(text);
   mi->func = func;
   menu = eina_list_append(menu, mi);
   evas_object_raise(o_menu_icon_sel2);
}

static void
_ui_setup(void)
{
   Evas_Object *o;
   Evas_Coord x, y, w, h;

   o = evas_object_rectangle_add(evas);
   evas_object_move(o, 0, 0);
   evas_object_resize(o, win_w, win_h);
   evas_object_color_set(o, 0, 0, 0, 0);
   evas_object_layer_set(o, 1000);
   evas_object_event_callback_add(o, EVAS_CALLBACK_KEY_DOWN, _ui_key, NULL);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_DOWN, _ui_mouse_down, NULL);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_UP, _ui_mouse_up, NULL);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_MOVE, _ui_mouse_move, NULL);
   evas_object_focus_set(o, 1);
   evas_object_show(o);
   o_bg = o;

   o = evas_object_rectangle_add(evas);
   evas_object_move(o, 0, 0);
   evas_object_resize(o, win_w, win_h);
   evas_object_color_set(o, 255, 255, 255, 255);
   evas_object_layer_set(o, -99);
   evas_object_show(o);
   o_wallpaper = o;

/*
   o = evas_object_image_add(evas);
   evas_object_move(o, 0, 0);
   evas_object_image_file_set(o, PACKAGE_DATA_DIR"/data/bg.png", NULL);
   evas_object_image_fill_set(o, 0, 0, win_w, win_h);
   evas_object_image_smooth_scale_set(o, 0);
   evas_object_resize(o, win_w, win_h);
   evas_object_layer_set(o, -99);
   evas_object_show(o);
   o_wallpaper = o;
 */

   o = evas_object_text_add(evas);
   evas_object_text_font_set(o, "Vera-Bold", 10);
   evas_object_text_text_set(o, "EXPEDITE");
   evas_object_layer_set(o, 100);
   evas_object_color_set(o, 0, 0, 0, 100);
   evas_object_pass_events_set(o, 1);
   evas_object_geometry_get(o, NULL, NULL, &w, &h);
   x = (win_w - w) / 2;
   y = 0;
   evas_object_move(o, x, y);
   evas_object_show(o);
   o_title = o;

   o = evas_object_text_add(evas);
   evas_object_text_font_set(o, "Vera", 9);
   evas_object_text_text_set(o, "LEFT/RIGHT - select, ENTER - select, ESCAPE - exit.");
   evas_object_layer_set(o, 100);
   evas_object_color_set(o, 0, 0, 0, 60);
   evas_object_pass_events_set(o, 1);
   evas_object_geometry_get(o, NULL, NULL, &w, NULL);
   x = (win_w - w) / 2;
   y = h + 2;
   evas_object_move(o, x, y);
   evas_object_show(o);
   o_byline = o;

   o = evas_object_image_add(evas);
   evas_object_move(o, (win_w - 120) / 2, ((win_h - 160) / 2));
   evas_object_image_file_set(o, build_path("logo.png"), NULL);
   evas_object_image_fill_set(o, 0, 0, 120, 160);
   evas_object_resize(o, 120, 160);
   evas_object_layer_set(o, -98);
   evas_object_color_set(o, 255, 255, 255, 255);
   evas_object_show(o);
   o_menu_logo = o;

   o = evas_object_image_add(evas);
   evas_object_move(o, win_w - 128, - 128);
   evas_object_image_fill_set(o, 0, 0, 256, 256);
   evas_object_resize(o, 256, 256);
   evas_object_show(o);
   o_menu_icon = o;

   o = evas_object_image_add(evas);
   evas_object_move(o, 0, 0);
   evas_object_image_file_set(o, build_path("icon_sel.png"), NULL);
   evas_object_resize(o, 56, 56);
   evas_object_image_fill_set(o, 0, 0, 56, 56);
   o_menu_icon_sel = o;

   o = evas_object_image_add(evas);
   evas_object_move(o, 0, 0);
   evas_object_image_file_set(o, build_path("text_sel.png"), NULL);
   evas_object_resize(o, 96, 32);
   evas_object_image_fill_set(o, 0, 0, 96, 32);
   evas_object_image_border_set(o, 8, 8, 8, 8);
   o_menu_text_sel = o;

   o = evas_object_image_add(evas);
   evas_object_move(o, 0, 0);
   evas_object_image_file_set(o, build_path("icon_sel2.png"), NULL);
   evas_object_resize(o, 56, 41);
   evas_object_image_fill_set(o, 0, 0, 56, 41);
   o_menu_icon_sel2 = o;

   o = evas_object_text_add(evas);
   evas_object_text_font_set(o, "Vera-Bold", 10);
   evas_object_text_text_set(o, "");
   evas_object_color_set(o, 100, 100, 100, 100);
   evas_object_pass_events_set(o, 1);
   evas_object_geometry_get(o, NULL, NULL, &w, &h);
   x = ((win_w - w) / 2) + 1;
   y = ((win_h - h) / 2) + 1;
   evas_object_move(o, x, y);
   o_menu_title2 = o;

   o = evas_object_text_add(evas);
   evas_object_text_font_set(o, "Vera-Bold", 10);
   evas_object_text_text_set(o, "");
   evas_object_color_set(o, 0, 0, 0, 100);
   evas_object_pass_events_set(o, 1);
   evas_object_geometry_get(o, NULL, NULL, &w, &h);
   x = (win_w - w) / 2;
   y = (win_h - h) / 2;
   evas_object_move(o, x, y);
   o_menu_title = o;

   _ui_menu_item_add("e.png", "About Enlightenment", about_start);
   _ui_menu_item_add("e.png", "All Tests", _ui_all);
#define UI
#include "tests.h"
#undef UI
   _ui_menu_item_add("exit.png", "Exit", _ui_exit);

   if (run_all)
     {
	_ui_all();
     }
   else if (run_test > 0)
     {
        _ui_num(run_test);
     }
   else if (list_test > 0)
     {
        Eina_List *l;
        int i;

        for (l = menu, i = -1; l; l = l->next, i++)
          {
             Menu_Item *mi;

             mi = l->data;
             if (i > 0)
               printf("%3i - %s\n", i, mi->text);
          }
     }
   else
     {
	menu_active = 1;
     }

   if (exit_after_test)
    _ui_exit();

}

void
ui_args(int argc, char **argv)
{
   int i;

   for (i = 1; i < argc; i++)
     {
	if (!strcmp(argv[i], "-a"))
	  {
	     run_all = 1;
         exit_after_test = 1;
	  }
	else if ((!strcmp(argv[i], "-t")) && (i < (argc - 1)))
	  {
             run_test = atoi(argv[i + 1]) + 1;
             exit_after_test = 1;
             if (run_test < 2) run_test = 2;
	  }
	else if (!strcmp(argv[i], "-l"))
	  {
             list_test = 1;
	  }
     }
   _ui_setup();
   start = get_time();
}

void
ui_loop(void)
{
   static int first = 1;
   static double pt = 0.0;
   double t, t2;

   if (loop_func)
     {
	t = get_time();
	f_loop++;
	f_start++;
	if ((t - t_loop) >= 1.0)
	  {
//	     ui_fps((double)f_loop / (t - t_loop));
	     t_loop = t;
	     f_loop = 0;
	  }
	loop_func(t - t_start, f_start);
	return;
     }
   t2 = get_time();
   if (first)
     {
	t = 0.1;
	pt = t2;
     }
   else
     {
	t = t2 - pt;
	pt = t2;
     }
   first = 0;

   /* menu layout */
   if (menu_active)
     {
	Eina_List *l;
	int i;
	static double tr = 0.0;
	double tt;

	tt = t;
	tt += tr;
	while (tt > 0.001)
	  {
	     menu_anim = (menu_anim * 0.995) + (menu_anim_sel * 0.005);
	     tt -= 0.001;
	  }
	tr = tt;
	for (i = 0, l = menu; l; l = l->next, i++)
	  {
	     char buf[4096];
	     Menu_Item *mi;
	     Evas_Coord x, y, w, h, tw, th;
	     Evas_Coord len;
	     double a;
	     Evas_Object *o;

	     mi = l->data;
	     o = mi->o_icon;
	     evas_object_geometry_get(o_menu_logo, NULL, NULL, &w, &h);
	     len = ((w * 3) + 10) / 4;
	     evas_object_geometry_get(o, NULL, NULL, &w, &h);
	     x = (win_w / 2)
	       + (sin((menu_anim - (double)i) * 0.33) * len)
		 - (w / 2);
	     y = (win_h / 2)
	       + (cos((menu_anim - (double)i) * 0.33) * len)
		 - (h / 2);
	     evas_object_move(o, x, y);
	     a = menu_anim - (double)i;
	     if (a < 0) a = -a;
	     a = 255 - (30 * a);
	     evas_object_color_set(o, a, a, a, a);
	     evas_object_show(o);


	     if (i == menu_sel)
	       {
		  a = menu_anim - (double)i;
		  if (a < 0) a = -a;
		  a = 255 - (255 * a);

		  o = o_menu_icon_sel;
		  evas_object_move(o, x - ((56 - w) / 2), y - ((56 - h) / 2));
		  evas_object_color_set(o, a, a, a, a);

		  o = o_menu_icon_sel2;
		  evas_object_move(o, (win_w - 56) / 2, (win_h / 2) + len - 4);

		  o = o_menu_title;
		  evas_object_color_set(o, 0, 0, 0, a);
		  evas_object_text_text_set(o, mi->text);
		  evas_object_geometry_get(o, NULL, NULL, &tw, &th);
	          x = (win_w - tw) / 2;
	          y = (win_h / 2) + len + 40;
	          evas_object_move(o, x, y);

		  o = o_menu_title2;
		  evas_object_color_set(o, a / 2, a / 2, a / 2, a / 2);
		  evas_object_text_text_set(o, mi->text);
	          evas_object_move(o, x + 1, y + 1);

		  o = o_menu_text_sel;
		  w = tw + 24;
		  h = 28;
		  x = x - 12;
		  y = y + ((th - h) / 2);
		  evas_object_move(o, x, y);
		  evas_object_resize(o, w, h);
		  evas_object_image_fill_set(o, 0, 0, w, h);
		  evas_object_color_set(o, a, a, a, a);

		  o = o_menu_icon;
		  snprintf(buf, 4096, "%s%s", data_dir, mi->icon);
		  evas_object_image_file_set(o, buf, NULL);
		  evas_object_color_set(o, a / 2, a / 2, a / 2, a / 2);
	       }
	  }
        evas_object_move(o_menu_logo, (win_w - 120) / 2, ((win_h - 160) / 2));
	evas_object_show(o_menu_logo);
	evas_object_show(o_menu_title);
	evas_object_show(o_menu_title2);
	evas_object_show(o_menu_icon);
	evas_object_show(o_menu_icon_sel);
	evas_object_show(o_menu_icon_sel2);
	evas_object_show(o_menu_text_sel);
     }
   else
     {
     }
   evas_object_resize(o_bg, win_w, win_h);
   evas_object_resize(o_wallpaper, win_w, win_h);
}

void
ui_menu(void)
{
   evas_object_text_text_set(o_byline, "LEFT/RIGHT - select, ENTER - select, ESCAPE - exit.");
   menu_active = 1;
   key_func = NULL;
   loop_func = NULL;
}

void
ui_func_set(void (*kfunc) (char *key), void (*lfunc) (double t, int f))
{
   key_func = kfunc;
   loop_func = lfunc;
   t_loop = t_start = get_time();
   f_loop = f_start = 0;
   ui_fps(0.0);
}


void
ui_fps(double fps)
{
   char buf[256];

   snprintf(buf, sizeof(buf), "ESCAPE - exit, FPS: %4.3f", fps);
   evas_object_text_text_set(o_byline, buf);
   p_fps = fps;
}
