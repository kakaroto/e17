#include "Evas.h"
#include "src/Etox.h"
#include "etox-config.h"
#include <stdio.h>
#include <stdlib.h>

#define IMGDIR PACKAGE_DATA_DIR"/img/"
#define FNTDIR PACKAGE_DATA_DIR"/fnt/"
#define STLDIR PACKAGE_DATA_DIR"/style/"

Etox et;
char txt[4096];

double down_x, down_y;
double ox, oy;

void
mouse_down(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
   evas_put_data(_e, _o, "clicked", (void *)1);
   evas_set_layer(_e, _o, 200);
   evas_get_geometry(_e, _o, &ox, &oy, NULL, NULL);
   down_x = evas_screen_x_to_world(_e, _x);
   down_y = evas_screen_y_to_world(_e, _y);
}

void
mouse_up(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
   Etox_Obstacle ob;
   double x, y, w, h;
   
   evas_remove_data(_e, _o, "clicked");
   evas_set_layer(_e, _o, 50);
   
   ob = _data;
   
   if (_b == 2)
     {
	etox_obstacle_del(et, ob);
	evas_hide(_e, _o);
	return;
     }
   x = evas_screen_x_to_world(_e, _x);
   y = evas_screen_y_to_world(_e, _y);   
   
   evas_move(_e, _o, ox + x - down_x, oy + y - down_y);
   evas_get_geometry(_e, _o, NULL, NULL, &w, &h); 
   
   etox_obstacle_set(et, ob, ox + x - down_x, oy + y - down_y, w, h);
}

void
mouse_move(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
  if (evas_get_data(_e, _o, "clicked"))
    {
       double x, y;
       
       x = evas_screen_x_to_world(_e, _x);
       y = evas_screen_y_to_world(_e, _y);   
       evas_move(_e, _o, ox + x - down_x, oy + y - down_y);
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
  Etox_Color ec, ec2;
  Etox_Obstacle ob[8];
  int win_w, win_h;

  Display *disp;
  Window win;

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
		   216, 8 * 1024 * 1024, 2 * 1024 * 1024,
		   FNTDIR);
  win = evas_get_window(e);
  XSelectInput(disp, win, ButtonPressMask | ButtonReleaseMask |
	       PointerMotionMask | ExposureMask | StructureNotifyMask);
  XMapWindow(disp, win);

  etox_style_add_path(PACKAGE_DATA_DIR"/etox/style");
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

  ec2 = etox_color_new();
  etox_color_set_member(ec2, "fg", 255, 255, 255, 255);
  etox_color_set_member(ec2, "ol", 10, 10, 10, 255);
  etox_color_set_member(ec2, "sh", 20, 20, 20, 100);

  st = etox_style_new("sh_ol");

  et = etox_new_all(e, "Test Etox", 10, 0, 
                    win_w - 20, win_h, 255, 5.0,
                    ETOX_ALIGN_TYPE_CENTER, ETOX_ALIGN_TYPE_CENTER, 
                    NULL, NULL, "nationff", 10, NULL);

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

  etox_set_text(et,ET_FONT("cinema", 16), ET_COLOR(ec2),
                   ET_TEXT("The Etox Test Program\n"), 
                   ET_FONT_END, ET_COLOR_END,
                   ET_TEXT("Etox is a text layout abstraction, built "
                           "on top of Evas. It is to text, what Ebits "
                           "is to images. It is intended to abstract "
                           "text layout to allow different fonts, colors, "
                           "styles (outline, shadowed, etc.), word "
                           "wrapping, paragraph layout, columnation and "
                           "wrapping of text around obstacles in the text "
                           "area.\n\nTry moving the images (obstacles) "
                           "around, and see how etox perfectly wraps the "
                           "text around them. Middle-clicking an image "
                           "will remove it..\n\n"),
/*                   ET_ALIGN(ETOX_ALIGN_TYPE_CENTER, ETOX_ALIGN_TYPE_LEFT),
*/
                   ET_TEXT("About E17:\n\nE17 is already beginning to use "
                           "a powerful object model system. I like to call "
                           "this system the \"fork() & exec()\" object model. "
                           "Unlike other desktops who like to invent new "
                           "\"interesting\" ways of doing object models, E17 "
                           "shall be using one that is the most widley used "
                           "object model in existance, one that has been in "
                           "heavy use and development for over 30 years. It "
                           "involves using 2 system calls - fork() and exec() "
                           "(and their variants). It's fast, powerful, "
                           "compatible, and requires no changes in existing "
                           "programs for them to work with this model. It's "
                           "great. "),
/*                   ET_ALIGN(ETOX_ALIGN_TYPE_CENTER, ETOX_ALIGN_TYPE_RIGHT),
*/
                   ET_TEXT("E17 is definitely being worked on - it's just "
                           "a bit quiet. It currently does just enough to "
                           "manage client windows - not all of ICCCM at all, "
                           "but just enough. I'm currently working on getting "
                           "the icon view working in viewing directories and "
                           "letting me at the least click on files. After "
                           "that it'll be time to throw in the DND support "
                           "and then we'll be cooking with gas. I plan on "
                           "recycling the icon/directory view for handling "
                           "iconified windows, application launcher panels, "
                           "configuration panels (background selection and "
                           "more) and general file browsing and "
                           "management.\n\n"), ET_END);

  etox_show(et);

  /* at test.. */
/*
  {
    int x, y, w, h;
    etox_get_at(et, 544, &x, &y, &w, &h); 
    printf("At Test: x = %d, y = %d, w = %d, h = %d\n", x, y, w, h);
    etox_get_at_position(et, 12, 294, &x, &y, &w, &h);
    printf("At Posiotion Test: x = %d, y = %d, w = %d, h = %d\n", x, y, w, h);
  }  
*/
  /* actual test.. */
/*
  {
    double w, h;
    etox_get_size(et, &w, &h);
    printf("Size: w = %f, h = %f\n", w, h);
    etox_get_actual_size(et, &w, &h);
    printf("Actual Size: w = %f, h = %f\n", w, h);  
  }
*/

  down = 0;
  for (;;)
    {
      XEvent ev;

      do
	{
	  XNextEvent(disp, &ev);
	  switch (ev.type)
	    {
	     case ConfigureNotify:
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
