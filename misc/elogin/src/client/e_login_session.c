#include"e_login_session.h"
#include"callbacks.h"
#include <Evas_Engine_Software_X11.h>

extern void intro_init(E_Login_Session e);

#define RENDER_METHOD RENDER_METHOD_ALPHA_SOFTWARE

/**
 * e_login_session_new: allocate a new  E_Login_Session
 * Returns a valid E_Login_Session
 * Also Allocates the auth, and parse the config struct 
 */
E_Login_Session
e_login_session_new(void)
{
   E_Login_Session e;

   e = (E_Login_Session) malloc(sizeof(struct _E_Login_Session));

   e->auth = e_login_auth_new();
   e->config =
      e_login_config_parse(PACKAGE_DATA_DIR "/data/config/elogin_config.db");

   return (e);
}

/**
 * e_login_session_free: free the elogin session
 */
void
e_login_session_free(E_Login_Session e)
{
   if (e)
   {
      e_login_auth_free(e->auth);
      e_login_config_free(e->config);
      evas_object_del(e->pointer);
      e_bg_free(e->bg);
      evas_free(e->evas);
      ecore_window_destroy(e->main_win);
      free(e);
   }
}

/**
 * e_login_session_init: Initialize the session by taking over the screen
 * @e - the E_Login_Session to be initialized
 */
void
e_login_session_init(E_Login_Session e)
{
   Evas_Engine_Info_Software_X11 *einfo;
   XSetWindowAttributes att;
   Window window;
   Display *disp;
   Window win, ewin;
   Evas *evas;
   Evas_List *l;
   Evas_Object *li;
   int iw, ih, ix, iy;

   if (!e)
      exit(1);

#if X_TESTING
   win = ecore_window_new(0, 0, 0, 800, 600);
   ecore_window_set_events(win, XEV_CONFIGURE | XEV_PROPERTY);
   ecore_window_set_name_class(win, "Elogin Test", "Main");
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

   evas_object_image_cache_set(evas, 0);
   evas_object_font_cache_set(evas, 0);
   evas_object_font_path_append(evas, PACKAGE_DATA_DIR "/data/fonts/");

   ecore_window_show(ewin);
   ecore_window_show(win);
   ecore_set_blank_pointer(win);

   /* try config */
   if ((e->config) && (e->config->bg))
      e->bg = e_bg_load(e->config->bg);
   if (!e->bg)
      e->bg = e_bg_load(PACKAGE_DATA_DIR "/data/bgs/elogin.bg.db");
   if (!e->bg)
      e->bg = e_bg_new();

   e_bg_add_to_evas(e->bg, evas);
   e_bg_resize(e->bg, e->geom.w, e->geom.h);
   e_bg_set_layer(e->bg, 0);
   e_bg_show(e->bg);

   e->pointer = evas_object_image_add(evas);
   evas_object_image_file_set(e->pointer,
                              PACKAGE_DATA_DIR "/data/images/pointer.png",
                              NULL);
   evas_object_image_size_get(e->pointer, &iw, &ih);
   evas_object_resize(e->pointer, iw, ih);
   evas_object_image_fill_set(e->pointer, 0.0, 0.0, (double) iw, (double) ih);
   evas_object_layer_set(e->pointer, 2000);
   evas_object_show(e->pointer);

   /* Session list background image */
   e->listbg = evas_object_image_add(evas);
   evas_object_image_file_set(e->listbg,
                              PACKAGE_DATA_DIR "/data/images/parch.png",
                              NULL);
   evas_object_image_size_get(e->listbg, &iw, &ih);
   evas_object_resize(e->listbg, iw, ih);
   evas_object_image_fill_set(e->listbg, 0.0, 0.0, (double) iw, (double) ih);
   evas_object_layer_set(e->listbg, 1);
   evas_object_move(e->listbg, 280.0, 40.0);
   evas_object_show(e->listbg);

   /* Session list heading */
   e->listhead = evas_object_text_add(evas);
   evas_object_text_font_set(e->listhead, "notepad.ttf", 21.0);
   evas_object_text_text_set(e->listhead, "Select Session");
   evas_object_layer_set(e->listhead, 2);
   evas_object_color_set(e->listhead, 90, 60, 25, 255);
   evas_object_move(e->listhead, 300.0, 80.0);
   evas_object_show(e->listhead);

   /* Build session list */
   ix = 334;
   iy = 120;

   e->listitems = NULL;
   for (l = e->config->sessions; l && iy <= 330; l = l->next)
   {
      li = evas_object_text_add(evas);
      evas_object_text_font_set(li, "notepad.ttf", 16.0);
      evas_object_text_text_set(li, (char *) evas_list_data(l));
      evas_object_layer_set(li, 5);
      evas_object_color_set(li, 0, 0, 0, 255);
      evas_object_move(li, (double) ix, (double) iy);
      evas_object_show(li);
      e->listitems = evas_list_append(e->listitems, li);
      iy += 30;
   }

   /* Bullet */
   e->bullet = evas_object_image_add(evas);
   evas_object_image_file_set(e->bullet,
                              PACKAGE_DATA_DIR "/data/images/bullet.png",
                              NULL);
   evas_object_image_size_get(e->bullet, &iw, &ih);
   evas_object_resize(e->bullet, iw, ih);
   evas_object_image_fill_set(e->bullet, 0.0, 0.0, (double) iw, (double) ih);
   evas_object_layer_set(e->bullet, 5);
   evas_object_move(e->bullet, 300, 120);
   evas_object_show(e->bullet);

   /* Set default session to first in list (for now) */ ;
   l = e->config->sessions;
   if (l)
      e->session = evas_list_data(l);
   else
      e->session = NULL;

   e->session_index = 0;

   e->evas = evas;
   e->ewin = ewin;
   e->main_win = win;
   intro_init(e);
}

/**
 * e_login_session_auth_user: attempt to authenticate the user
 * Returns 0 on success errors otherwise
 */
int
e_login_session_auth_user(E_Login_Session e)
{
   return (e_login_auth_cmp(e->auth));
}

/**
 * e_login_session_auth_user: forget what we know about the current user
 */
void
e_login_session_reset_user(E_Login_Session e)
{
   e_login_auth_free(e->auth);
   e->auth = e_login_auth_new();
}
