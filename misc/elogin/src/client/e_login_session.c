#include"e_login_session.h"
#include"callbacks.h"

extern void intro_init(E_Login_Session e);

#define RENDER_METHOD RENDER_METHOD_ALPHA_SOFTWARE

E_Login_Session
e_login_session_new(void)
{
   E_Login_Session e;

   e = (E_Login_Session) malloc(sizeof(struct _E_Login_Session));

   e->auth = e_login_auth_new();

   return (e);
}

void
e_login_session_init(E_Login_Session e)
{
   Window win, ewin;
   Evas evas;
   char *bgfile = PACKAGE_DATA_DIR "/data/bgs/elogin.bg.db";
   int iw, ih;

#if X_TESTING
   win = ecore_window_new(0, 0, 0, 640, 480);
   ecore_window_set_events(win, XEV_CONFIGURE | XEV_PROPERTY);
   ecore_window_set_name_class(win, "Elogin Test", "Main");
   e->geom.w = 640;
   e->geom.h = 480;
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
   e->ewin = ewin;
   e->evas = evas;

   e->bg = e_bg_load(bgfile);
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

int
e_login_session_auth_user(E_Login_Session e)
{
   return (e_login_auth_cmp(e->auth));
}

void
e_login_session_reset_user(E_Login_Session e)
{
   e_login_auth_free(e->auth);
   e->auth = e_login_auth_new();
}
