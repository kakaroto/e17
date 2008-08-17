#include "main.h"

#include <xcb/xcb.h>
#include <xcb/xcb_keysyms.h>
#include <Evas_Engine_Software_Xcb.h>

#define XK_MISCELLANY
#include <X11/keysymdef.h>

enum {
   XCB_SIZE_US_POSITION_HINT = 1 << 0,
   XCB_SIZE_US_SIZE_HINT = 1 << 1,
   XCB_SIZE_P_POSITION_HINT = 1 << 2,
   XCB_SIZE_P_SIZE_HINT = 1 << 3,
   XCB_SIZE_P_MIN_SIZE_HINT = 1 << 4,
   XCB_SIZE_P_MAX_SIZE_HINT = 1 << 5,
   XCB_SIZE_P_RESIZE_INC_HINT = 1 << 6,
   XCB_SIZE_P_ASPECT_HINT = 1 << 7,
   XCB_SIZE_BASE_SIZE_HINT = 1 << 8,
   XCB_SIZE_P_WIN_GRAVITY_HINT = 1 << 9
};

struct xcb_size_hints_t {
   uint32_t flags;
   int32_t  x, y, width, height;
   int32_t  min_width, min_height;
   int32_t  max_width, max_height;
   int32_t  width_inc, height_inc;
   int32_t  min_aspect_num, min_aspect_den;
   int32_t  max_aspect_num, max_aspect_den;
   int32_t  base_width, base_height;
   uint32_t win_gravity;
};


static xcb_connection_t *conn = NULL;
static xcb_screen_t     *screen = NULL;
static xcb_window_t      win = 0;

int
engine_software_xcb_args(int argc, char **argv)
{
   struct xcb_size_hints_t        hints;
   uint32_t                       value_list[6];
   xcb_screen_iterator_t          iter;
   Evas_Engine_Info_Software_Xcb *einfo;
   xcb_intern_atom_reply_t       *reply;
   char                          *str;
   xcb_intern_atom_cookie_t       cookie1;
   xcb_intern_atom_cookie_t       cookie2;
   xcb_intern_atom_cookie_t       cookie3;
   xcb_intern_atom_cookie_t       cookie4;
   xcb_intern_atom_cookie_t       cookie5;
   xcb_atom_t                     string;
   xcb_atom_t                     wm_name;
   xcb_atom_t                     wm_class;
   xcb_atom_t                     wm_normal_hint;
   xcb_atom_t                     wm_size_hint;
   uint32_t                       value_mask;
   int                            s;
   int                            l1;
   int                            l2;
   int                            i;
   int                            ok = 0;

   for (i = 1; i < argc; i++)
     {
	if ((!strcmp(argv[i], "-e")) && (i < (argc - 1)))
	  {
	     i++;
	     if (!strcmp(argv[i], "xcb")) ok = 1;
	  }
     }
   if (!ok) return 0;

   conn = xcb_connect(NULL, &s);
   if (!conn) return 0;

   iter = xcb_setup_roots_iterator(xcb_get_setup(conn));
   for (; iter.rem; --s, xcb_screen_next (&iter))
     if (s == 0)
       {
          screen = iter.data;
          break;
       }

   evas_output_method_set(evas, evas_render_method_lookup("software_xcb"));
   einfo = (Evas_Engine_Info_Software_Xcb *)evas_engine_info_get(evas);
   if (!einfo)
     {
	printf("Evas does not support the Software XCB Engine\n");
	return 0;
     }

   einfo->info.conn = conn;
   einfo->info.screen = screen;
   einfo->info.visual = einfo->func.best_visual_get(screen);
   einfo->info.colormap = einfo->func.best_colormap_get(screen);
   einfo->info.depth = einfo->func.best_depth_get(screen);
   einfo->info.rotation = 0;
   einfo->info.debug = 0;

   value_mask =
     XCB_CW_BACK_PIXMAP   | XCB_CW_BORDER_PIXEL | XCB_CW_BIT_GRAVITY |
     XCB_CW_BACKING_STORE | XCB_CW_EVENT_MASK   | XCB_CW_COLORMAP;

   value_list[0]  = XCB_NONE;
   value_list[1]  = 0;
   value_list[2]  = XCB_GRAVITY_BIT_FORGET;
   value_list[3]  = XCB_BACKING_STORE_NOT_USEFUL;
   value_list[4]  =
     XCB_EVENT_MASK_KEY_PRESS       | XCB_EVENT_MASK_KEY_RELEASE    |
     XCB_EVENT_MASK_BUTTON_PRESS    | XCB_EVENT_MASK_BUTTON_RELEASE |
     XCB_EVENT_MASK_ENTER_WINDOW    | XCB_EVENT_MASK_LEAVE_WINDOW   |
     XCB_EVENT_MASK_POINTER_MOTION  | XCB_EVENT_MASK_EXPOSURE       |
     XCB_EVENT_MASK_STRUCTURE_NOTIFY;
   value_list[5]  = einfo->info.colormap;

   win = xcb_generate_id(conn);
   xcb_create_window(conn,
                     einfo->info.depth,
                     win, screen->root, 0, 0, win_w, win_h, 0,
                     XCB_WINDOW_CLASS_INPUT_OUTPUT,
                     einfo->info.visual->visual_id,
                     value_mask,
                     value_list);
   einfo->info.drawable = win;

   xcb_map_window(conn, win);

   evas_engine_info_set(evas, (Evas_Engine_Info *) einfo);

/*    XStoreName(disp, win, "Expedite - Evas Test Suite"); */

   cookie1 = xcb_intern_atom_unchecked(conn, 0, strlen("STRING"), "STRING");
   cookie2 = xcb_intern_atom_unchecked(conn, 0, strlen("WM_NAME"), "WM_NAME");
   cookie3 = xcb_intern_atom_unchecked(conn, 0, strlen("WM_CLASS"), "WM_CLASS");
   cookie4 = xcb_intern_atom_unchecked(conn, 0, strlen("WM_NORMAL_HINTS)"), "WM_NORMAL_HINTS)");
   cookie5 = xcb_intern_atom_unchecked(conn, 0, strlen("WM_SIZE_HINTS)"), "WM_SIZE_HINTS)");

   reply = xcb_intern_atom_reply(conn, cookie1, NULL);
   string = reply->atom;
   free(reply);
   reply = xcb_intern_atom_reply(conn, cookie2, NULL);
   wm_name = reply->atom;
   free(reply);

   xcb_change_property(conn, XCB_PROP_MODE_REPLACE, win,
                       wm_name, string, 8,
                       strlen("Expedite - Evas Test Suite"), "Expedite - Evas Test Suite");

/*    chint.res_name = "expedite"; */
/*    chint.res_class = "Expedite"; */
/*    XSetClassHint(disp, win, &chint); */

   l1 = strlen("expedite");
   l2 = strlen("Expedite");
   str = (char *)malloc(l1 + l2 + 1);
   memcpy(str, "expedite", l1);
   str[l1] = '\0';
   memcpy(str + l1 + 1, "Expedite", l2);

   reply = xcb_intern_atom_reply(conn, cookie3, NULL);
   wm_class = reply->atom;
   free(reply);

   xcb_change_property(conn, XCB_PROP_MODE_REPLACE, win,
                       wm_class, string, 8,
                       l1 + l2 + 1, str);
   free(str);

/*    szhints.flags = PMinSize | PMaxSize | PSize | USSize; */
/*    szhints.min_width = szhints.max_width = win_w; */
/*    szhints.min_height = szhints.max_height = win_h; */
/*    XSetWMNormalHints(disp, win, &szhints); */

   memset(&hints, 0, sizeof(hints));
   hints.flags = XCB_SIZE_US_SIZE_HINT | XCB_SIZE_P_SIZE_HINT | XCB_SIZE_P_MIN_SIZE_HINT | XCB_SIZE_P_MAX_SIZE_HINT;
   hints.min_width = win_w;
   hints.max_width = win_w;
   hints.min_height = win_h;
   hints.max_height = win_h;

   reply = xcb_intern_atom_reply(conn, cookie4, NULL);
   wm_normal_hint = reply->atom;
   free(reply);

   reply = xcb_intern_atom_reply(conn, cookie5, NULL);
   wm_size_hint = reply->atom;
   free(reply);

   xcb_change_property(conn, XCB_PROP_MODE_REPLACE, win,
                       wm_normal_hint, wm_size_hint, 32,
                       sizeof(hints) / 4, &hints);

   free(xcb_get_input_focus_reply(conn, xcb_get_input_focus_unchecked(conn), NULL));

   return 1;
}

void
engine_software_xcb_loop(void)
{
   xcb_generic_event_t *ev;

   again:
   ev = xcb_poll_for_event(conn);
   if (!ev)
     return;

   switch (ev->response_type)
     {
      case XCB_BUTTON_PRESS:
        {
           xcb_button_press_event_t *e;

           e = (xcb_button_press_event_t *)ev;

           evas_event_feed_mouse_move(evas, e->event_x, e->event_y, 0, NULL);
           evas_event_feed_mouse_down(evas, e->detail, EVAS_BUTTON_NONE, 0, NULL);
           break;
        }
      case XCB_BUTTON_RELEASE:
        {
           xcb_button_release_event_t *e;

           e = (xcb_button_release_event_t *)ev;

           evas_event_feed_mouse_move(evas, e->event_x, e->event_y, 0, NULL);
           evas_event_feed_mouse_up(evas, e->detail, EVAS_BUTTON_NONE, 0, NULL);
           break;
        }
      case XCB_MOTION_NOTIFY:
        {
           xcb_motion_notify_event_t *e;

           e = (xcb_motion_notify_event_t *)ev;

           evas_event_feed_mouse_move(evas, e->event_x, e->event_y, 0, NULL);
           break;
        }
      case XCB_EXPOSE:
        {
           xcb_expose_event_t *e;

           e = (xcb_expose_event_t *)ev;

           evas_damage_rectangle_add(evas,
                                     e->x,
                                     e->y,
                                     e->width,
                                     e->height);
           break;
        }
      case XCB_CONFIGURE_NOTIFY:
        {
           xcb_configure_notify_event_t *e;

           e = (xcb_configure_notify_event_t *)ev;

           evas_output_size_set(evas,
                                e->width,
                                e->height);
           break;
        }
      case XCB_ENTER_NOTIFY:
         evas_event_feed_mouse_in(evas, 0, NULL);
         break;
      case XCB_LEAVE_NOTIFY:
         evas_event_feed_mouse_out(evas, 0, NULL);
         break;
      case XCB_KEY_PRESS:
        {
           xcb_key_press_event_t *e;
           xcb_key_symbols_t *kss;
           xcb_keysym_t       ks;
           char              *str;

           e = (xcb_key_press_event_t *)ev;

           kss = xcb_key_symbols_alloc(conn);

           ks = xcb_key_symbols_get_keysym(kss, e->detail, 0);

           if ((ks == XK_Shift_L) || (ks == XK_Shift_R))
             evas_key_modifier_on(evas, "Shift");
           else if ((ks == XK_Control_L) || (ks == XK_Control_R))
             evas_key_modifier_on(evas, "Control");
           else if ((ks == XK_Alt_L) || (ks == XK_Alt_R))
             evas_key_modifier_on(evas, "Alt");
           else if ((ks == XK_Super_L) || (ks == XK_Super_R))
             evas_key_modifier_on(evas, "Super");
           else if (ks == XK_Caps_Lock)
             {
                if (evas_key_lock_is_set(evas_key_lock_get(evas), "Caps_Lock"))
                  evas_key_lock_off(evas, "Caps_Lock");
                else
                  evas_key_lock_on(evas, "Caps_Lock");
             }
           else if (ks == XK_Scroll_Lock)
             {
                if (evas_key_lock_is_set(evas_key_lock_get(evas), "Scroll_Lock"))
                  evas_key_lock_off(evas, "Scroll_Lock");
                else
                  evas_key_lock_on(evas, "Scroll_Lock");
             }

           if (ks == XK_Left)
             str = "Left";
           if (ks == XK_Right)
             str = "Right";
           if (ks == XK_Escape)
             str = "Escape";
           if (ks == XK_Return)
             str = "Return";
           if (ks == 71)
             str = "q";

           evas_event_feed_key_down(evas, str, str, NULL, NULL, 0, NULL);

           xcb_key_symbols_free(kss);
           break;
        }
      case XCB_KEY_RELEASE:
        {
           xcb_key_release_event_t *e;
           xcb_key_symbols_t *kss;
           xcb_keysym_t       ks;
           char              *str;

           e = (xcb_key_release_event_t *)ev;

           kss = xcb_key_symbols_alloc(conn);

           ks = xcb_key_symbols_get_keysym(kss, e->detail, 0);

           if ((ks == XK_Shift_L) || (ks == XK_Shift_R))
             evas_key_modifier_on(evas, "Shift");
           else if ((ks == XK_Control_L) || (ks == XK_Control_R))
             evas_key_modifier_on(evas, "Control");
           else if ((ks == XK_Alt_L) || (ks == XK_Alt_R))
             evas_key_modifier_on(evas, "Alt");
           else if ((ks == XK_Super_L) || (ks == XK_Super_R))
             evas_key_modifier_on(evas, "Super");

           if (ks == XK_Left)
             str = "Left";
           if (ks == XK_Right)
             str = "Right";
           if (ks == XK_Escape)
             str = "Escape";
           if (ks == XK_Return)
             str = "Return";
           if (ks == 71)
             str = "q";

           evas_event_feed_key_up(evas, str, str, NULL, NULL, 0, NULL);

           xcb_key_symbols_free(kss);
           break;
        }
      default:
         free(ev);
         return;
     }
   free(ev);

   goto again;
}
