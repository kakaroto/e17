#include"entrance_session.h"
#include"sys_callbacks.h"
#include "ui_callbacks.h"
#include <Evas_Engine_Software_X11.h>

extern void intro_init(Entrance_Session e);

#define RENDER_METHOD RENDER_METHOD_ALPHA_SOFTWARE

/**
 * entrance_session_new: allocate a new  Entrance_Session
 * Returns a valid Entrance_Session
 * Also Allocates the auth, and parse the config struct 
 */
Entrance_Session
entrance_session_new(void)
{
   Entrance_Session e;
   char theme_path[PATH_MAX];

   e = (Entrance_Session) malloc(sizeof(struct _Entrance_Session));

   e->auth = entrance_auth_new();
   e->config =
      entrance_config_parse(PACKAGE_DATA_DIR
                            "/data/config/entrance_config.db");
   if (!e->config)
   {
      fprintf(stderr,
              "Entrance: Fatal Error: Unable to read configuration.\n");
      exit(1);
   }
   snprintf(theme_path, PATH_MAX, PACKAGE_DATA_DIR "/data/themes/%s",
            e->config->theme);
   e->theme = entrance_theme_parse(e->config->theme, theme_path);
   if (!e->theme)
   {
      fprintf(stderr,
              "Entrance: Fatal Error: Unable to load specified theme.\n");
      exit(1);
   }
   e->theme->name = strdup(e->config->theme);
   e->theme->path = strdup(theme_path);

   return (e);
}


/**
 * entrance_session_free: free the entrance session
 */
void
entrance_session_free(Entrance_Session e)
{
   if (e)
   {
      entrance_auth_free(e->auth);
      entrance_config_free(e->config);
      evas_object_del(e->pointer);
      e_bg_free(e->bg);
      evas_free(e->evas);
      ecore_window_destroy(e->main_win);
      free(e);
   }
}

static void
init_session_list(Entrance_Session e)
{
   int ix, iy, i = 0;
   double px, py;
   Evas_List *l;
   Evas_Object *li;
   Entrance_Theme t = e->theme;
   char buf[PATH_MAX];

   px =
      (t->session_list.pos.x * (double) e->geom.w) +
      (double) t->session_list.offset.x;
   py =
      (t->session_list.pos.y * (double) e->geom.h) +
      (double) t->session_list.offset.y;

   /* Build session list */
   ix = (int) (px + 20.0);
   iy = (int) (py + 20.0);

   /* TODO: Add a theme option for session list margin (as if we don't have
      enough theme options! */

   e->listitems = NULL;
/*   for(l = e->config->sessions; 
       l && iy <= (int) py + t->session_list.size.h 
                           - 20 - t->session_list.font.size;
       l = l->next) */
   for (l = e->config->sessions; l; l = l->next)
   {
      char *session_name =
         ((Entrance_Session_Type *) evas_list_data(l))->name;
      li = evas_object_text_add(e->evas);
      snprintf(buf, PATH_MAX, "session_list_item_%d", i);
      evas_object_name_set(li, buf);
      THEME_SETFONT(li, t->session_list);
      THEME_SETCOLOR(li, t->session_list.color);
      evas_object_text_text_set(li, session_name);
      evas_object_layer_set(li, 25);
      evas_object_move(li, (double) ix, (double) iy);
      evas_object_resize(li, (double) t->session_list.size.w - 40.0,
                         (double) t->session_list.font.size + 7.0);
      /* Callbacks */
      evas_object_event_callback_add(li, EVAS_CALLBACK_MOUSE_UP,
                                     session_list_clicked_cb, e);
      evas_object_event_callback_add(li, EVAS_CALLBACK_MOUSE_IN,
                                     session_list_mousein_cb, e);
      evas_object_event_callback_add(li, EVAS_CALLBACK_MOUSE_OUT,
                                     session_list_mouseout_cb, e);
      e->listitems = evas_list_append(e->listitems, li);
      iy += e->theme->session_list.font.size + 8;
      ++i;
   }

   /* Session List Box */
   e->listbox = evas_object_rectangle_add(e->evas);
   evas_object_name_set(e->listbox, "session_list_box");
   evas_object_move(e->listbox, px, py);
   iy = 40 + (t->session_list.font.size + 8) * evas_list_count(e->listitems);
   evas_object_resize(e->listbox, (double) t->session_list.size.w,
                      (double) iy);
   THEME_SETCOLOR(e->listbox, t->session_list.box_color);
   evas_object_layer_set(e->listbox, 4);

   /* Set default session */ ;
   l = e->config->sessions;
   if (e->config->default_session)
      e->session = e->config->default_session;
   else if (l)
      e->session = evas_list_data(l);
   else
      e->session = NULL;

   e->session_index = e->config->default_index;
}

static void
init_session_selector(Entrance_Session e)
{
   Entrance_Theme t = e->theme;

   /* Selected Session Text */
   e->sessiontext = evas_object_text_add(e->evas);
   THEME_SETFONT(e->sessiontext, t->selected_session.text);
   evas_object_text_text_set(e->sessiontext, e->session->name);
   evas_object_layer_set(e->sessiontext, 9);
   evas_object_name_set(e->sessiontext, "session_selector");
   THEME_SETCOLOR(e->sessiontext, t->selected_session.text.color);
   THEME_MOVE(e->sessiontext, t->selected_session.text, e->geom);
   evas_object_event_callback_add(e->sessiontext, EVAS_CALLBACK_MOUSE_UP,
                                  session_selector_clicked_cb, e);
   evas_object_event_callback_add(e->sessiontext, EVAS_CALLBACK_MOUSE_IN,
                                  session_selector_mousein_cb, e);
   evas_object_event_callback_add(e->sessiontext, EVAS_CALLBACK_MOUSE_OUT,
                                  session_selector_mouseout_cb, e);
   evas_object_show(e->sessiontext);

   e->sessionicon = evas_object_image_add(e->evas);
   evas_object_name_set(e->sessionicon, "session_icon");
   evas_object_image_file_set(e->sessionicon, e->session->icon, NULL);
   THEME_MOVE(e->sessionicon, t->selected_session.icon, e->geom);
   THEME_IMAGE_RESIZE(e->sessionicon, t->selected_session.icon);
   evas_object_layer_set(e->sessionicon, 9);
   evas_object_show(e->sessionicon);
}

static void
init_login_face(Entrance_Session e)
{
   Entrance_Theme t = e->theme;

   e->face = evas_object_image_add(e->evas);
   evas_object_name_set(e->face, "login_face");
   THEME_MOVE(e->face, t->face, e->geom);
   THEME_IMAGE_RESIZE(e->face, t->face);
   evas_object_layer_set(e->face, 20);
   evas_object_image_border_set(e->face, t->face.border, t->face.border,
                                t->face.border, t->face.border);
   /* FIXME: Darn, yet another theme option that needs to be added */
   evas_object_color_set(e->face, 255, 255, 255, 255);

   /* Shadow */
   e->face_shadow = evas_object_rectangle_add(e->evas);
   evas_object_name_set(e->face_shadow, "login_face_shadow");
   THEME_RESIZE(e->face_shadow, t->face);
   evas_object_move(e->face_shadow,
                    (t->face.pos.x * (double) e->geom.w) +
                    (double) t->face.offset.x + 5,
                    (t->face.pos.y * (double) e->geom.h) +
                    (double) t->face.offset.y + 5);
   evas_object_color_set(e->face_shadow, 0, 0, 0, 128);
   evas_object_layer_set(e->face_shadow, 19);
}

static void
init_info_section(Entrance_Session e)
{
   Entrance_Theme t = e->theme;

   /* Greeting message/hostname */
   e->hostname = evas_object_text_add(e->evas);
   evas_object_text_text_set(e->hostname, e->config->greeting);
   evas_object_name_set(e->hostname, "info_hostname");
   THEME_SETFONT(e->hostname, t->hostname);
   THEME_SETCOLOR(e->hostname, t->hostname.color);
   THEME_MOVE(e->hostname, t->hostname, e->geom);
   evas_object_layer_set(e->hostname, 10);
   evas_object_show(e->hostname);

   /* Date */
   e->date = evas_object_text_add(e->evas);
   evas_object_name_set(e->date, "info_date");
   evas_object_text_text_set(e->date, "");
   THEME_SETFONT(e->date, t->date);
   THEME_SETCOLOR(e->date, t->date.color);
   THEME_MOVE(e->date, t->date, e->geom);
   evas_object_layer_set(e->date, 10);
   evas_object_show(e->date);
   ecore_add_event_timer("date_update", 0.1, entrance_update_time_cb, 0,
                         e->date);

   /* Time */
   e->time = evas_object_text_add(e->evas);
   evas_object_name_set(e->time, "info_time");
   evas_object_text_text_set(e->time, "");
   THEME_SETFONT(e->time, t->time);
   THEME_SETCOLOR(e->time, t->time.color);
   THEME_MOVE(e->time, t->time, e->geom);
   evas_object_layer_set(e->time, 10);
   evas_object_show(e->time);
   ecore_add_event_timer("time_update", 0.1, entrance_update_time_cb, 1,
                         e->time);
}

static void
entrance_window_prepare(Entrance_Session e)
{
   int iw, ih;

   e->bg = NULL;

   /* Set up the background */
   /* Try theme first */
   if ((e->theme) && (e->theme->bg))
      e->bg = e_bg_load(e->theme->bg);
   /* Then try default */
   if (!e->bg)
      e->bg = e_bg_load(PACKAGE_DATA_DIR "/data/bgs/entrance.bg.db");
   /* Bilious barnacles! Blank background. */
   if (!e->bg)
      e->bg = e_bg_new();

   e_bg_add_to_evas(e->bg, e->evas);
   e_bg_resize(e->bg, e->geom.w, e->geom.h);
   e_bg_set_layer(e->bg, 0);
   e_bg_show(e->bg);

   /* Graphical Pointer */
   e->pointer = evas_object_image_add(e->evas);
   evas_object_image_file_set(e->pointer, e->theme->pointer, NULL);
   evas_object_image_size_get(e->pointer, &iw, &ih);
   evas_object_resize(e->pointer, iw, ih);
   evas_object_image_fill_set(e->pointer, 0.0, 0.0, (double) iw, (double) ih);
   evas_object_layer_set(e->pointer, 2000);
   evas_object_pass_events_set(e->pointer, 1);
   evas_object_show(e->pointer);
}


/**
 * entrance_session_init: Initialize the session by taking over the screen
 * @e - the Entrance_Session to be initialized
 */
void
entrance_session_init(Entrance_Session e)
{
   Evas_Engine_Info_Software_X11 *einfo;
   XSetWindowAttributes att;
   Window window;
   Display *disp;
   Window win, ewin;
   Evas *evas;

   if (!e)
      exit(1);

#if X_TESTING
   win = ecore_window_new(0, 0, 0, 800, 600);
   ecore_window_set_events(win, XEV_CONFIGURE | XEV_PROPERTY);
   ecore_window_set_name_class(win, "Entrance Test", "Main");
   e->geom.w = 800;
   e->geom.h = 600;
#else
   ecore_window_get_geometry(ecore_window_root(), NULL, NULL, &e->geom.w,
                             &e->geom.h);
   win = ecore_window_root();
#endif

   ecore_window_move(win, 0, 0);

   evas = evas_new();
   evas_output_method_set(evas, evas_render_method_lookup("software_x11"));
   evas_output_size_set(evas, e->geom.w, e->geom.h);
   evas_output_viewport_set(evas, 0, 0, e->geom.w, e->geom.h);


   /* the following is specific to the engine */
   einfo = (Evas_Engine_Info_Software_X11 *) evas_engine_info_get(evas);
   disp = ecore_display_get();

   einfo->info.display = disp;
   einfo->info.visual = DefaultVisual(disp, DefaultScreen(disp));
   einfo->info.colormap = DefaultColormap(disp, DefaultScreen(disp));
   att.background_pixmap = None;
   att.colormap = DefaultColormap(disp, DefaultScreen(disp));
   att.border_pixel = 0;
   att.event_mask = 0;
   window =
      XCreateWindow(disp, win, 0, 0, e->geom.w, e->geom.h, 0,
                    DefaultDepth(disp, DefaultScreen(disp)), InputOutput,
                    einfo->info.visual,
                    CWColormap | CWBorderPixel | CWEventMask | CWBackPixmap,
                    &att);

   einfo->info.drawable = window;
   einfo->info.depth = DefaultDepth(disp, DefaultScreen(disp));
   einfo->info.rotation = 0;
   einfo->info.debug = 0;
   evas_engine_info_set(evas, (Evas_Engine_Info *) einfo);

   ewin = einfo->info.drawable;
   ecore_window_set_events(ewin,
                           XEV_EXPOSE | XEV_BUTTON | XEV_MOUSE_MOVE |
                           XEV_KEY);

   evas_image_cache_set(evas, 0);
   evas_font_cache_set(evas, 0);
   evas_font_path_append(evas, PACKAGE_DATA_DIR "/data/fonts/");
   if (e->theme && e->theme->path)
      evas_font_path_append(evas, e->theme->path);

   ecore_window_show(ewin);
   ecore_window_show(win);
   ecore_set_blank_pointer(win);
   e->evas = evas;

   /* Initialize the UI */
   entrance_window_prepare(e);
   init_session_list(e);
   init_session_selector(e);
   init_login_face(e);
   init_info_section(e);

   e->ewin = ewin;
   e->main_win = win;
   intro_init(e);
}

/**
 * entrance_session_auth_user: attempt to authenticate the user
 * Returns 0 on success errors otherwise
 */
int
entrance_session_auth_user(Entrance_Session e)
{
   return (entrance_auth_cmp(e->auth));
}

/**
 * entrance_session_auth_user: forget what we know about the current user
 */
void
entrance_session_reset_user(Entrance_Session e)
{
   entrance_auth_free(e->auth);
   e->auth = entrance_auth_new();
}
