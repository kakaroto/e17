#include "main.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <Evas_Engine_Software_X11.h>

static Display *disp = NULL;
static Window win = 0;

int
engine_software_x11_args(int argc, char **argv)
{
   XSetWindowAttributes attr;
   XClassHint chint;
   XSizeHints szhints;
   Evas_Engine_Info_Software_X11 *einfo;
   int i;
   int ok = 0;

   for (i = 1; i < argc; i++)
     {
	if ((!strcmp(argv[i], "-e")) && (i < (argc - 1)))
	  {
	     i++;
	     if (!strcmp(argv[i], "x11")) ok = 1;
	  }
     }
   if (!ok) return 0;

   disp = XOpenDisplay(NULL);
   if (!disp) return 0;

   evas_output_method_set(evas, evas_render_method_lookup("software_x11"));
   einfo = (Evas_Engine_Info_Software_X11 *)evas_engine_info_get(evas);
   if (!einfo)
     {
	printf("Evas does not support the Software X11 Engine\n");
	return 0;
     }

   einfo->info.backend = EVAS_ENGINE_INFO_SOFTWARE_X11_BACKEND_XLIB;
   einfo->info.connection = disp;
   einfo->info.screen = NULL;
   einfo->info.visual = einfo->func.best_visual_get(0, disp, DefaultScreen(disp));
   einfo->info.colormap = einfo->func.best_colormap_get(0, disp, DefaultScreen(disp));
   einfo->info.depth = einfo->func.best_depth_get(0, disp, DefaultScreen(disp));
   einfo->info.rotation = 0;
   einfo->info.debug = 0;
   attr.backing_store = NotUseful;
   attr.colormap = einfo->info.colormap;
   attr.border_pixel = 0;
   attr.background_pixmap = None;
   attr.event_mask =
     ExposureMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask |
     StructureNotifyMask | EnterWindowMask | LeaveWindowMask |
     KeyPressMask | KeyReleaseMask;
   attr.bit_gravity = ForgetGravity;
   win = XCreateWindow(disp, DefaultRootWindow(disp),
		       0, 0, win_w, win_h, 0,
		       einfo->info.depth, InputOutput,
		       einfo->info.visual,
		       CWBackingStore | CWColormap |
		       CWBackPixmap | CWBorderPixel |
		       CWBitGravity | CWEventMask,
		       &attr);
   einfo->info.drawable = win;
   if (!evas_engine_info_set(evas, (Evas_Engine_Info *) einfo))
     {
	printf("Evas can not setup the informations of the Software X11 Engine\n");
        return 0;
     }

   XStoreName(disp, win, "Expedite - Evas Test Suite");
   chint.res_name = "expedite";
   chint.res_class = "Expedite";
   XSetClassHint(disp, win, &chint);
   szhints.flags = PMinSize | PMaxSize | PSize | USSize;
   szhints.min_width = szhints.max_width = win_w;
   szhints.min_height = szhints.max_height = win_h;
   XSetWMNormalHints(disp, win, &szhints);
   XMapWindow(disp, win);
   XSync(disp, False);
   return 1;
}

void
engine_software_x11_loop(void)
{
   XEvent ev;
   KeySym ks;
   char *kstr;

   again:
   if (!XCheckMaskEvent(disp,
			ExposureMask |
			StructureNotifyMask |
			KeyPressMask |
			KeyReleaseMask |
			ButtonPressMask |
			ButtonReleaseMask |
			PointerMotionMask |
			EnterWindowMask |
			LeaveWindowMask, &ev))
     return;
   switch (ev.type)
     {
      case ButtonPress:
	evas_event_feed_mouse_move(evas, ev.xbutton.x, ev.xbutton.y, 0, NULL);
	evas_event_feed_mouse_down(evas, ev.xbutton.button, EVAS_BUTTON_NONE, 0, NULL);
	break;
      case ButtonRelease:
	evas_event_feed_mouse_move(evas, ev.xbutton.x, ev.xbutton.y, 0, NULL);
	evas_event_feed_mouse_up(evas, ev.xbutton.button, EVAS_BUTTON_NONE, 0, NULL);
	break;
      case MotionNotify:
	evas_event_feed_mouse_move(evas, ev.xmotion.x, ev.xmotion.y, 0, NULL);
	break;
      case Expose:
	evas_damage_rectangle_add(evas,
				  ev.xexpose.x,
				  ev.xexpose.y,
				  ev.xexpose.width,
				  ev.xexpose.height);
	break;
      case ConfigureNotify:
        evas_output_viewport_set(evas, 0, 0,
                                 ev.xconfigure.width,
                                 ev.xconfigure.height);
	evas_output_size_set(evas,
			     ev.xconfigure.width,
			     ev.xconfigure.height);
        win_w = ev.xconfigure.width;
        win_h = ev.xconfigure.height;
	break;
      case EnterNotify:
	evas_event_feed_mouse_in(evas, 0, NULL);
	break;
      case LeaveNotify:
	evas_event_feed_mouse_out(evas, 0, NULL);
	break;
      case KeyPress:
	ks = XKeycodeToKeysym(ev.xkey.display, ev.xkey.keycode, 0);
	kstr = XKeysymToString(ks);
	if (kstr)
	  {
	     if ((!strcmp(kstr, "Shift_L")) ||
		 (!strcmp(kstr, "Shift_R")))
	       evas_key_modifier_on(evas, "Shift");
	     if ((!strcmp(kstr, "Control_L")) ||
		 (!strcmp(kstr, "Control_R")))
	       evas_key_modifier_on(evas, "Control");
	     if ((!strcmp(kstr, "Alt_L")) ||
		 (!strcmp(kstr, "Alt_R")))
	       evas_key_modifier_on(evas, "Alt");
	     if ((!strcmp(kstr, "Super_L")) ||
		 (!strcmp(kstr, "Super_R")))
	       evas_key_modifier_on(evas, "Super");
	     if (!strcmp(kstr, "Caps_Lock"))
	       {
		  if (evas_key_lock_is_set(evas_key_lock_get(evas), "Caps_Lock"))
		    evas_key_lock_off(evas, "Caps_Lock");
		  else
		    evas_key_lock_on(evas, "Caps_Lock");
	       }
	     if (!strcmp(kstr, "Num_Lock"))
	       {
		  if (evas_key_lock_is_set(evas_key_lock_get(evas), "Num_Lock"))
		    evas_key_lock_off(evas, "Num_Lock");
		  else
		    evas_key_lock_on(evas, "Num_Lock");
	       }
	     if (!strcmp(kstr, "Scroll_Lock"))
	       {
		  if (evas_key_lock_is_set(evas_key_lock_get(evas), "Scroll_Lock"))
		    evas_key_lock_off(evas, "Scroll_Lock");
		  else
		    evas_key_lock_on(evas, "Scroll_Lock");
	       }
	     if (kstr) evas_event_feed_key_down(evas, kstr, kstr, NULL, NULL, 0, NULL);
	  }
	break;
      case KeyRelease:
	ks = XKeycodeToKeysym(ev.xkey.display, ev.xkey.keycode, 0);
	kstr = XKeysymToString(ks);
	if (kstr)
	  {
	     if ((!strcmp(kstr, "Shift_L")) ||
		 (!strcmp(kstr, "Shift_R")))
	       evas_key_modifier_off(evas, "Shift");
	     if ((!strcmp(kstr, "Control_L")) ||
		 (!strcmp(kstr, "Control_R")))
	       evas_key_modifier_off(evas, "Control");
	     if ((!strcmp(kstr, "Alt_L")) ||
		 (!strcmp(kstr, "Alt_R")))
	       evas_key_modifier_off(evas, "Alt");
	     if ((!strcmp(kstr, "Super_L")) ||
		 (!strcmp(kstr, "Super_R")))
	       evas_key_modifier_off(evas, "Super");
	     evas_event_feed_key_up(evas, kstr, kstr, NULL, NULL, 0, NULL);
	  }
	break;
      default:
	break;
     }
   goto again;
}
