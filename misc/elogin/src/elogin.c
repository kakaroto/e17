#include "elogin.h"
#include "auth.h"

/* funcs */
static void elogin_display_init(char *display);
static void elogin_view_init(void);
static void elogin_setup(void);
static void elogin_create_bg(void);
static void elogin_create_entries(void);
static void elogin_idle(void *data);
static void elogin_window_expose(Eevent *ev);
static void elogin_mouse_down(Eevent *ev);
static void elogin_key_down(Eevent *ev);
static void elogin_key_up(Eevent *ev);
static void elogin_handle_mouse_down(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y);
static void elogin_exit_clean(void);
Elogin_Greeter *elogin_greeter_new(void);

/* globals */
Evas elogin_main;
Evas_Render_Method render_method = RENDER_METHOD_ALPHA_SOFTWARE;
Evas_Object o_bg;
Elogin_Greeter *greeter;
E_Entry *entry;
int screen_width = 0;
int screen_height = 0;
char *greet_img = "/home/chris/cvs/e17/misc/elogin/img/elogin_greeter.png";
char *bg_img = "/home/chris/cvs/e17/misc/elogin/img/bg.png";

#define USERNAME "yourusernamehere"
#define PASSWORD "yourpasswordhere"

/* main */
int main(int argc, char **argv)
{
   printf("Starting Elogin on display %s\n", argv[1]);

   elogin_display_init((char *)argv[1]);
   elogin_view_init();
   elogin_setup();
   e_event_loop();

   return 0;
}

static void elogin_display_init(char *display)
{
   if (!e_display_init(display))
     {
	printf("eeeek. can't connect to %s\n", display);
	exit(1);
     }
   e_ev_signal_init();
   e_event_filter_init();
   /* e_timeout(0, NULL); */
   e_ev_x_init();
}

static void elogin_view_init(void)
{
   Display *dsp = e_display_get();
   Window bg, ebg;
   char *font_dir = e_config_get("fonts");

   e_event_filter_handler_add(EV_WINDOW_EXPOSE, elogin_window_expose);
   e_event_filter_handler_add(EV_MOUSE_DOWN, elogin_mouse_down);
   e_event_filter_handler_add(EV_KEY_DOWN, elogin_key_down);
   e_event_filter_handler_add(EV_KEY_UP, elogin_key_up);
   e_event_filter_idle_handler_add(elogin_idle, NULL);


   screen_width = DisplayWidth(dsp, DefaultScreen(dsp));
   screen_height = DisplayHeight(dsp, DefaultScreen(dsp));

   bg = DefaultRootWindow(dsp);
   elogin_main = evas_new_all(dsp,
	 bg,
	 0, 0, screen_width, screen_height,
	 render_method,
	 216,
	 1 * (1024 * 1024),
	 8 * (1024 * 1024),
	 font_dir);
   ebg = evas_get_window(elogin_main);
   e_window_set_events(ebg, XEV_MOUSE_MOVE | XEV_BUTTON | XEV_EXPOSE | XEV_KEY);
   e_window_show(ebg);

   e_window_show(bg);
}

static void elogin_setup(void)
{
   elogin_create_bg();
   elogin_create_entries();
   greeter = elogin_greeter_new();

   evas_raise(elogin_main, entry);
   evas_lower(elogin_main, greeter->greet_win);
}

Elogin_Greeter *elogin_greeter_new(void)
{
   Elogin_Greeter *tmp;

   tmp = malloc(sizeof(Elogin_Greeter));
   tmp->greet_win = evas_add_image_from_file(elogin_main, greet_img);
   tmp->user_input = NULL;
   tmp->pass_input = NULL;
   tmp->w = 500;
   tmp->h = 350;

   evas_callback_add(elogin_main, tmp->greet_win, CALLBACK_MOUSE_DOWN, elogin_handle_mouse_down, NULL);
   evas_move(elogin_main, tmp->greet_win, (screen_width - tmp->w) / 2, (screen_height - tmp->h) / 2);
   evas_set_layer(elogin_main, tmp, 100);
   evas_show(elogin_main, tmp->greet_win);

   return tmp;
}

static void elogin_create_bg(void)
{
   o_bg = evas_add_image_from_file(elogin_main, bg_img);
   evas_resize(elogin_main, o_bg, screen_width, screen_height);
   evas_set_layer(elogin_main, o_bg, 100);
   evas_show(elogin_main, o_bg);
}

static void elogin_create_entries(void)
{
   entry = e_entry_new();
   e_entry_set_evas(entry, elogin_main);
   e_entry_set_layer(entry, 100);
   e_entry_move(entry, 700, 300);
   e_entry_resize(entry, 100, 24);
   e_entry_set_focus(entry, 1);
   e_entry_show(entry);
   e_entry_set_text(entry, "Some Test Entry Text... Play with me!");
   {
      int ew, eh;

      e_entry_query_max_size(entry, &ew, &eh);
      e_entry_resize(entry, ew, eh);
   }
}

static void elogin_idle(void *data)
{
   evas_render(elogin_main);
}

static void elogin_window_expose(Eevent *ev)
{
   Ev_Window_Expose *e;
   
   e = (Ev_Window_Expose *)ev->event;
   evas_update_rect(elogin_main, e->x, e->y, e->w, e->h);
}

static void elogin_mouse_down(Eevent *ev)
{
   Ev_Mouse_Down *e;

   e = (Ev_Mouse_Down *)ev->event;
   evas_event_button_down(elogin_main, e->x, e->y, e->button);
}

static void elogin_key_down(Eevent *ev)
{
   Ev_Key_Down *e;
   char *type;

   e = ev->event;

   if (entry)
      e_entry_handle_keypress(entry, e);
}

static void elogin_key_up(Eevent *ev)
{
   Ev_Key_Up *e;
   
   e = ev->event;

}

static void elogin_handle_mouse_down(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
   switch(_b)
   {
      case 1:
	 printf("Mouse button 1 was clicked\n");
	 break;
      case 2:
	 printf("Mouse button 2 was clicked\n");
	 if ((elogin_auth_user(USERNAME, PASSWORD)) == SUCCESS)
	 {
	    Userinfo *u = elogin_new_userinfo(USERNAME);

	    elogin_start_client(u);
	 }
	 elogin_auth_cleanup();
	 break;
      case 3:
	 printf("Exiting\n");
	 elogin_exit_clean();
	 break;
      default:
	 break;
   }
}

static void elogin_exit_clean(void)
{
   evas_free(elogin_main);
   free(o_bg);
   free(greeter);

   exit(0);
}

char * elogin_greeter_msg (char *str)
{
   return str;
}
