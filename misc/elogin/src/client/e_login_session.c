#include"e_login_session.h"
#include"callbacks.h"

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
      evas_del_object(e->evas, e->pointer);
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
   Window win, ewin;
   Evas evas;
   int iw, ih;

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
   evas =
      evas_new_all(ecore_display_get(), win, 0, 0, e->geom.w, e->geom.h,
                   RENDER_METHOD, 215, (1024 * 1024) * 4, (1024 * 1024) * 6,
                   PACKAGE_DATA_DIR "/data/fonts/");

   ewin = evas_get_window(evas);

   ecore_window_set_events(ewin,
                           XEV_EXPOSE | XEV_BUTTON | XEV_MOUSE_MOVE | XEV_KEY
                           | XEV_IN_OUT);

   ecore_window_show(ewin);
   ecore_window_show(win);
   ecore_set_blank_pointer(win);

   e->main_win = win;
   e->evas = evas;

   /* try config */
   if ((e->config) && (e->config->bg))
      e->bg = e_bg_load(e->config->bg);
   if (!e->bg)
      e->bg = e_bg_load(PACKAGE_DATA_DIR "/data/bgs/elogin.bg.db");
   if (!e->bg)
      e->bg = e_bg_new();

   e_bg_add_to_evas(e->bg, e->evas);
   e_bg_resize(e->bg, e->geom.w, e->geom.h);
   e_bg_set_layer(e->bg, 0);
   e_bg_show(e->bg);

   e->pointer =
      evas_add_image_from_file(evas,
                               PACKAGE_DATA_DIR "/data/images/pointer.png");
   evas_get_image_size(evas, e->pointer, &iw, &ih);
   evas_resize(evas, e->pointer, iw, ih);
   evas_set_image_fill(evas, e->pointer, 0.0, 0.0, (double) iw, (double) ih);
   evas_set_layer(evas, e->pointer, 2000);
   evas_show(evas, e->pointer);
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
