#include "Evas.h"
#include "src/Etox.h"
#include <stdio.h>
#include <stdlib.h>

#define IMGDIR DATADIR"/etox/img/"
#define FNTDIR DATADIR"/etox/fnt"
#define STLDIR DATADIR"/etox/style/"

Etox et;
char txt[4096];

void
mouse_down (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
  evas_put_data(_e, _o, "clicked", (void *)1);
  evas_put_data(_e, _o, "x", (void *)_x);
  evas_put_data(_e, _o, "y", (void *)_y);
  evas_set_layer(_e, _o, 200);
}

void
mouse_up (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
  Etox_Obstacle ob;
  double ox, oy, w, h; 
  int x, y;

  evas_remove_data(_e, _o, "clicked");
  evas_set_layer(_e, _o, 50);

  ob = _data;

  if (_b == 2)
    {
      etox_obstacle_del(et, ob);
      evas_hide(_e, _o);
      return;
    }

  evas_get_geometry(_e, _o, &ox, &oy, &w, &h); 
  x = evas_get_data(_e, _o, "x"); 
  y = evas_get_data(_e, _o, "y");
  etox_obstacle_set(et, ob, (ox + _x - x) - 15, oy + _y - y, w + 10, h);
}

void
mouse_move (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
  if (evas_get_data(_e, _o, "clicked"))
    {                                  
      double ox, oy;
      int x, y;

      evas_get_geometry(_e, _o, &ox, &oy, NULL, NULL);
      x = evas_get_data(_e, _o, "x");
      y = evas_get_data(_e, _o, "y");
      evas_put_data(_e, _o, "x", (void *)_x);
      evas_put_data(_e, _o, "y", (void *)_y);
      evas_move(_e, _o, ox + _x - x, oy + _y - y);
    }                                              
}

int
main(int argc, char *argv[])
{
  Evas e;
  Evas_Object o[8];
  Evas_Render_Method method;
  int down;
  int x, y, w, h;

  Etox_Style st;
  Etox_Color ec;
  Etox_Obstacle ob[8];
  int win_w, win_h;

  Display *disp;
  Window win;

  char txt[4096];

  win_w = 640;
  win_h = 580;
   
  method = RENDER_METHOD_ALPHA_SOFTWARE;
  if (argc > 1)
    {
       if (!strcmp(argv[1], "x11")) method = RENDER_METHOD_BASIC_HARDWARE;
       if (!strcmp(argv[1], "soft")) method = RENDER_METHOD_ALPHA_SOFTWARE;
       if (!strcmp(argv[1], "hard")) method = RENDER_METHOD_3D_HARDWARE;
       if (!strcmp(argv[1], "render")) method = RENDER_METHOD_ALPHA_HARDWARE;
       if (!strcmp(argv[1], "-h"))
	 {
	    printf("options:\n"
		   "\t %s [x11 | soft | hard | render]\n"
		   "Where the option selects the evas rendering engine.\n",
		   argv[0]);
	    exit(0);
	 }
    }

  disp = XOpenDisplay(NULL);
  win = DefaultRootWindow(disp);
  e = evas_new_all(disp, win, 128, 0, win_w, win_h, method,
		   216, 8 * 1024 * 1024, 1 * 1024 * 1024,
		   FNTDIR);
  win = evas_get_window(e);
  XSelectInput(disp, win, ButtonPressMask | ButtonReleaseMask |
	       PointerMotionMask | ExposureMask | StructureNotifyMask);
  XMapWindow(disp, win);

  etox_style_add_path(DATADIR"/etox/style");
  etox_style_add_path("./style");

  o[0] = evas_add_rectangle(e);
  evas_set_color(e, o[0], 200, 200, 200, 255);
  evas_resize(e, o[0], win_w, win_h);
  evas_move(e, o[0], 0, 0);
  evas_lower(e, o[0]);
  evas_show(e, o[0]);

  ec = etox_color_new();
  etox_color_set_member(ec, "fg", 240, 240, 240, 255);
  etox_color_set_member(ec, "ol", 10, 10, 10, 255);
  etox_color_set_member(ec, "sh", 20, 20, 20, 100);

  et = etox_new_all(e, "Test Etox", 10, 0, win_w - 20, win_h,
		    "nationff", 10, "sh_ol.style", ec,
		    ETOX_ALIGN_CENTER, ETOX_ALIGN_BOTTOM,
		    255, 0);

  x = 60, y = 320;
  o[1] = evas_add_image_from_file(e, IMGDIR"evas_test_image_0.png");
  evas_get_image_size(e, o[1], &w, &h);
  ob[0] = etox_obstacle_add(et, x, y, w, h);
  evas_move(e, o[1], x, y); 
  evas_raise(e, o[1]);
  evas_show(e, o[1]);
  evas_callback_add(e, o[1], CALLBACK_MOUSE_DOWN, mouse_down, NULL);
  evas_callback_add(e, o[1], CALLBACK_MOUSE_UP, mouse_up, ob[0]);
  evas_callback_add(e, o[1], CALLBACK_MOUSE_MOVE, mouse_move, NULL);
  
  x = 200, y = 60;
  o[2] = evas_add_image_from_file(e, IMGDIR"evas_test_image_1.png");
  evas_get_image_size(e, o[2], &w, &h);
  ob[1] = etox_obstacle_add(et, x, y, w, h);
  evas_move(e, o[2], x, y); 
  evas_raise(e, o[2]);  
  evas_show(e, o[2]); 
  evas_callback_add(e, o[2], CALLBACK_MOUSE_DOWN, mouse_down, NULL); 
  evas_callback_add(e, o[2], CALLBACK_MOUSE_UP, mouse_up, ob[1]); 
  evas_callback_add(e, o[2], CALLBACK_MOUSE_MOVE, mouse_move, NULL);

  x = 400, y = 100;
  o[3] = evas_add_image_from_file(e, IMGDIR"evas_test_image_2.png");
  evas_get_image_size(e, o[3], &w, &h);
  ob[2] = etox_obstacle_add(et, x, y, w, h);
  evas_move(e, o[3], x, y); 
  evas_raise(e, o[3]);   
  evas_show(e, o[3]);  
  evas_callback_add(e, o[3], CALLBACK_MOUSE_DOWN, mouse_down, NULL);  
  evas_callback_add(e, o[3], CALLBACK_MOUSE_UP, mouse_up, ob[2]);  
  evas_callback_add(e, o[3], CALLBACK_MOUSE_MOVE, mouse_move, NULL);

  strcpy(txt, "~font=cinema~~size=16~~color=fg 255 240 180 255~The Etox Test Program\n");

  strcat(txt, "~font=notepad~~size=10~~color=fg 255 255 255 255~Etox is a text layout abstraction, built ");
  strcat(txt, "on top of Evas. It is to text, what Ebits is to images.");
  strcat(txt, " It is intended to abstract text layout to allow different");
  strcat(txt, " fonts, colors, styles (outline, shadowed, etc.), word ");
  strcat(txt, "wrapping, paragraph layout, columnation and wrapping of ");
  strcat(txt, "text around obstacles in the text area.\n\n");

  strcat(txt, "Try moving the images (obstacles) around, and see how ");
  strcat(txt, "etox perfectly wraps the text around them. Middle-clicking ");
  strcat(txt, "an image will remove it..\n\n");

  strcat(txt, "~align=left~About E17:\n\n");

  strcat(txt, "~size=10~~font=notepad~E17 is already beginning to use a powerful ");
  strcat(txt, "object model system. I like to call this system the ");
  strcat(txt, "\"fork() & exec()\" object model. Unlike other desktops ");
  strcat(txt, "who like to invent new \"interesting\" ways of doing ");
  strcat(txt, "object models, E17 shall be using one that is the most ");
  strcat(txt, "widley used object model in existance, one that has been ");
  strcat(txt, "in heavy use and development for over 30 years. It involves ");
  strcat(txt, "using 2 system calls - fork() and exec() (and their ");
  strcat(txt, "variants).It's fast, powerful, compatible, and requires no ");
  strcat(txt, "changes in existing programs for them to work with this ");
  strcat(txt, "model. It's great.\n\n");

  strcat(txt, "~align=right~E17 is definitely being worked on - it's just ");
  strcat(txt, "a bit quiet. It currently does just enough to manage client ");
  strcat(txt, "windows - not all of ICCCM at all, but just enough. I'm ");
  strcat(txt, "currently working on getting the icon view working in ");
  strcat(txt, "viewing directories and letting me at the least click on ");
  strcat(txt, "files. After that it'll be time to throw in the DND support ");
  strcat(txt, "and then we'll be cooking with gas. I plan on recycling the ");
  strcat(txt, "icon/directory view for handling iconified windows, ");
  strcat(txt, "application launcher panels, configuration panels ");
  strcat(txt, "(background selection and more) and general file browsing ");
  strcat(txt, "and management.\n\n");

  etox_set_text(et, txt);
  etox_show(et);

  down = 0;
  for (;;)
    {
      double x, y;
      XEvent ev;

      do
	{
	  XNextEvent(disp, &ev);
	  switch (ev.type)
	    {
	     case ConfigureNotify:
	       printf("resize to %i %i\n",
		      ev.xconfigure.width, 
		      ev.xconfigure.height);
	       evas_set_output_size(e, 
				    ev.xconfigure.width, 
				    ev.xconfigure.height);
	      case ButtonPress:
	        {
		  int button, mouse_x, mouse_y;

		  down = 1;
		  button = ev.xbutton.button;
		  mouse_x = ev.xbutton.x;    
		  mouse_y = ev.xbutton.y;
		  evas_event_button_down(e, mouse_x, mouse_y, button);
		}                                                      
	      break;
	      case ButtonRelease:
		{
		  int button, mouse_x, mouse_y;
                                                    
		  down = 0;                    
		  button = ev.xbutton.button;
		  if (button == 3)           
		    {             
		      evas_free(e);
		      exit(0);     
		    }          
		  mouse_x = ev.xbutton.x;
		  mouse_y = ev.xbutton.y;
		  evas_event_button_up(e, mouse_x, mouse_y, button);
		}                                                      
	      break;
	      case MotionNotify:
		{            
		  int mouse_x, mouse_y;

		  mouse_x = ev.xmotion.x;
		  mouse_y = ev.xmotion.y;
		  evas_event_move(e, mouse_x, mouse_y);
		}                                       
	      break;
	      case Expose:
		{      
		  evas_update_rect(e, ev.xexpose.x, ev.xexpose.y, 
				   ev.xexpose.width, ev.xexpose.height);
		}
		break;
	      default:
	      break; 
	    }
	}
       while (XPending(disp));
      evas_render(e);
    }

  return 0;
}
