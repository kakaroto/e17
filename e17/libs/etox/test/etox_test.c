#include "Evas.h"
#include "src/Etox.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <math.h>

#define IMGDIR DATADIR"/etox/img/"
#define FNTDIR DATADIR"/etox/fnt"
#define STLDIR DATADIR"/etox/style/"

#define NRECTS 3

double get_time(void);

double
get_time(void)
{
   struct timeval      timev;
   
   gettimeofday(&timev, NULL);
   return (double)timev.tv_sec + (((double)timev.tv_usec) / 1000000);
}

int
main(int argc, char **argv)
{
   Imlib_Image buffer = NULL;
   int win_w, win_h;
   int i, a, w, h, m;
   Evas e;
   Evas_Object o[3], rect[NRECTS], rect_2;

   Etox_Style  e_font_style;
   Etox        e_etox;
   Etox        e_etox_2;
   Etox        et_up_label;
   Etox        et_down_label;
   XSetWindowAttributes att;
   Evas_Render_Method method;
   Etox_Color  tcl;
   
   int down;
   double t1, t2;
   double etox_x, etox_y;
   int alpha_up_goal, alpha_down_goal;
     
   Display *disp;
   Window   win;
   Window   root_win;
   
   char txt[4096];
   
   win_w = 640; win_h = 480;
   /* the program... */

   /* connect to X */
   disp  = XOpenDisplay(NULL);
   /* get default visual , colormap etc. you could ask imlib2 for what it */
   /* thinks is the best, but this example is intended to be simple */
   root_win = DefaultRootWindow(disp);

   e = evas_new_all(disp, root_win, 128, 0, win_w, win_h,
		    RENDER_METHOD_ALPHA_SOFTWARE, 
		    216, 4 * 1024 * 1024, 4 * 1024 * 1024,
		    FNTDIR);

   win = evas_get_window(e);
   
   /* tell X what events we are interested in */
   XSelectInput(disp, win, 
		ButtonPressMask | ButtonReleaseMask | 
                PointerMotionMask | ExposureMask | 
		StructureNotifyMask | SubstructureNotifyMask |
		VisibilityChangeMask);
   XMapWindow(disp, win);

   etox_style_add_path(DATADIR"/etox/style");
   etox_style_add_path("./style");
   
   o[0] = evas_add_image_from_file(e, IMGDIR"sky001.png");
   evas_lower(e, o[0]);
   evas_show(e, o[0]);
   
   o[1] = evas_add_image_from_file(e, IMGDIR"arrow_up.png");
   evas_move(e, o[1], 500, 10);
   evas_set_layer(e, o[1], 50);
   evas_show(e, o[1]);
   
   o[2] = evas_add_image_from_file(e, IMGDIR"arrow_down.png");
   evas_move(e, o[2], 500, 400);
   evas_set_layer(e, o[2], 50);
   evas_show(e, o[2]);
   
   e_font_style = etox_style_new("sh_ol.style");
  
   e_etox = etox_new(e, "Etox name");
   etox_set_style(e_etox, e_font_style);
   etox_set_layer(e_etox, 10);
   etox_set_font(e_etox, "cinema", 10);
   etox_move(e_etox, 0, 0);
   etox_resize(e_etox, 450, 1000);

   etox_obstacle_add(e_etox, 100, 80, 50, 400);
   rect[0] = evas_add_rectangle(e);
   evas_set_color(e, rect[0], 20, 40, 200, 150);
   evas_resize(e, rect[0], 50, 400);
   evas_move(e, rect[0], 105, 80);
   evas_set_layer(e, rect[0], 10);
   evas_show(e, rect[0]);
   etox_obstacle_add(e_etox, 350, 150, 100, 100);
   rect[1] = evas_add_rectangle(e);
   evas_set_color(e, rect[1], 20, 200, 40, 150);
   evas_resize(e, rect[1], 100, 100);
   evas_move(e, rect[1], 355, 150);
   evas_set_layer(e, rect[1], 10);
   evas_show(e, rect[1]);

   strcpy(txt,"~color=fg 255 2 2 255~~color=ol 0 0 0 255~~color=sh 0 0 0 255~~valign=bottom~~font=morpheus~~size=20~");
   strcat(txt,"~align=center~~size=25~~font=cinema~Th~font=morpheus~~size=20~e Gospel of Tux (v1.0)\n\n");

   strcat(txt,"~font=nationff~~size=12~~color=fg 255 255 255 255~~align~In the beginning Turing created the Machine.\n\n");

   strcat(txt,"~align=center~And the Machine was crufty and bodacious, existing in ");
   strcat(txt,"theory only. And von Neumann looked upon the Machine, ");
   strcat(txt,"and saw that it was crufty. He divided the Machine ");
   strcat(txt,"into two Abstractions, the Data and the Code, and yet ");
   strcat(txt,"the two were one Architecture. This is a great ");
   strcat(txt,"Mystery, and the beginning of wisdom.\n\n");

   strcat(txt,"~align~~style=plain~And von Neumann spoke unto the Architecture, and ");
   strcat(txt,"blessed it, saying, \"Go forth and replicate, freely ");
   strcat(txt,"exchanging data and code, and bring forth all manner ");
   strcat(txt,"of devices unto the earth.\" And it was so, and it was ");
   strcat(txt,"cool. The Architecture prospered and was implemented ");
   strcat(txt,"in hardware and software. And it brought forth many ");
   strcat(txt,"Systems unto the earth.\n\n");
     

   strcat(txt,"~style~~align=right~The first Systems were mighty giants; many great ");
   strcat(txt,"works of renown did they accomplish. Among them were ");
   strcat(txt,"Colossus, the codebreaker; ENIAC, the targeter; EDSAC ");
   strcat(txt,"and MULTIVAC and all manner of froody creatures ");
   strcat(txt,"ending in AC, the experimenters; and SAGE, the ");
   strcat(txt,"defender of the sky and father of all networks. These ");
   strcat(txt,"were the mighty giants of old, the first children of ");
   strcat(txt,"Turing, and their works are written in the Books of ");
   strcat(txt,"the Ancients. This was the First Age, the age of ");
   strcat(txt,"Lore.\n\n");

   strcat(txt,"~align=left~Now the sons of Marketing looked upon the children of ");
   strcat(txt,"Turing, and saw that they were swift of mind and ");
   strcat(txt,"terse of name and had many great and baleful ");
   strcat(txt,"attributes. And they said unto themselves, \"Let us go ");
   strcat(txt,"now and make us Corporations, to bind the Systems to ");
   strcat(txt,"our own use that they may bring us great fortune.\" ");
   strcat(txt,"With sweet words did they lure their customers, and ");
   strcat(txt,"with many chains did they bind the Systems, to ");
   strcat(txt,"fashion them after their own image. And the sons of ");
   strcat(txt,"Marketing fashioned themselves Suits to wear, the ");
   strcat(txt,"better to lure their customers, and wrote grave and ");
   strcat(txt,"perilous Licenses, the better to bind the Systems. ");
   strcat(txt,"And the sons of Marketing thus became known as Suits, ");
   strcat(txt,"despising and being despised by the true Engineers, ");
   strcat(txt,"the children of von Neumann. ");
   
   etox_set_alpha(e_etox, 255);
   etox_set_text(e_etox, txt);
   etox_x = 5; etox_y = 0;
   etox_move(e_etox, etox_x, etox_y);
   etox_show(e_etox);
   
   rect[2] = evas_add_rectangle(e);
   evas_set_color(e, rect[2], 200, 250, 40, 70);
   evas_resize(e, rect[2], 450, 1000);
   evas_move(e, rect[2], etox_x, etox_y);
   evas_set_layer(e, rect[2], 10);
   evas_show(e, rect[2]);
   evas_lower(e, rect[2]);
   
   e_etox_2 = etox_new(e, "Showoff");
   etox_set_style(e_etox_2, e_font_style);
   etox_set_layer(e_etox_2, 15);
   etox_set_font(e_etox_2, "cinema", 10);
   etox_resize(e_etox_2, 210, 400);
   etox_set_alpha(e_etox_2, 100);

   strcpy(txt,"~color=fg 255 255 255 255~~color=ol 0 0 0 255~~color=sh 0 0 0 255~~valign=bottom~~font=notepad~~size=14~Various vertical and horizontal alignments:\n");
   strcat(txt,"~color=fg 255 2 2 255~~color=ol 0 0 0 255~~color=sh 0 0 0 255~~valign=bottom~~font=cinema~");
   strcat(txt,"~valign=top~~align=left~~size=25~B~size=23~B~size=21~B~size=19~B~size=17~B\n");
   strcat(txt,"~valign=center~~align=center~~size=25~B~size=23~B~size=21~B~size=19~B~size=17~B\n");
   strcat(txt,"~valign=bottom~~align=right~~size=25~B~size=23~B~size=21~B~size=19~B~size=17~B\n");
   
   etox_set_text(e_etox_2, txt);
   etox_set_layer(e_etox_2, 15);
   etox_move(e_etox_2, 420, 75);
   etox_show(e_etox_2);
  
   rect_2 = evas_add_rectangle(e);
   evas_set_color(e, rect_2, 200, 50, 25, 70);
   evas_resize(e, rect_2, 210, 400);
   evas_move(e, rect_2, 420, 75);
   evas_set_layer(e, rect_2, 14);
   evas_show(e, rect_2);

   tcl = etox_color_new();
   etox_color_set_member(tcl, "fg", 255, 255, 255, 255);
   etox_color_set_member(tcl, "ol", 20, 20, 100, 255);
   etox_color_set_member(tcl, "sh", 100, 30, 30, 255);
   
   et_up_label = etox_new_all(e, "UP label", 400, 5, 200, 100, 
			      "cinema", 15, "sh_ol.style", tcl,
			      ETOX_ALIGN_CENTER, ETOX_ALIGN_BOTTOM,
                              60, 0);
   etox_set_alpha(et_up_label, 0);
   etox_set_text(et_up_label, "GO UP !!!");   
   etox_show(et_up_label);
   
   et_down_label = etox_new_all(e, "UP label", 400, 430, 200, 100,
                                "cinema", 15, "sh_ol.style", tcl,
                                ETOX_ALIGN_CENTER, ETOX_ALIGN_BOTTOM,     
                                60, 0);
   etox_set_alpha(et_down_label, 0);
   etox_set_text(et_down_label, "GO DOWN !!!");   
   etox_show(et_down_label);
      
   evas_move(e, o[0], 0, 0);
   evas_resize(e, o[0], win_w, win_h);
   evas_set_image_fill(e, o[0], 0, 0, win_w, win_h);

   t1 = get_time();
   alpha_up_goal = 0;
   alpha_down_goal = 0;
   for (;;)
     {
	double x, y;
	XEvent              ev;


	while (XPending(disp))
	  {
	     XNextEvent(disp, &ev);
	     switch(ev.type)
	       {
		case ConfigureNotify:
		  /* window resized */
		  printf("resize to %i %i\n",
			 ev.xconfigure.width, 
			 ev.xconfigure.height);
		  evas_set_output_size(e, 
				       ev.xconfigure.width, 
				       ev.xconfigure.height);
		  break;	       
		case Expose:
		  /* window rectangle was exposed - add it to the list of */
		  /* rectangles we need to re-render */
		  evas_update_rect(e,
				   ev.xexpose.x, ev.xexpose.y,
				   ev.xexpose.width, ev.xexpose.height);
		  break;	       
		case ButtonRelease:
		    {
		       int button, mouse_x, mouse_y;
		       
		       down = 0;
		       button = ev.xbutton.button;
		       mouse_x = ev.xbutton.x;
		       mouse_y = ev.xbutton.y;
		       if (button == 1)
			 {
			    Evas_Object obj;
			    
			    obj = evas_object_at_position(e, mouse_x, mouse_y);

			    if (obj == o[1])
			      {
				 etox_y += 25;
				 alpha_up_goal = 255;
			      }
			    else if (obj == o[2])
			      {
				 etox_y -= 25;
				 alpha_down_goal = 255;
			      }
			 }
		       if (button == 3)
			 {
			    int i=0;
			    
			    etox_free(et_down_label);
			    etox_free(et_up_label);
			    etox_free(e_etox_2);
			    etox_free(e_etox);
			    etox_color_free(tcl);
			    for (i=0; i<3; i++)
			      evas_del_object(e, o[i]);
			    for (i=0; i<NRECTS; i++)
			      evas_del_object(e, rect[i]);
			    evas_del_object(e, rect_2);
			    
			    evas_free(e);
			    exit(0);
			 }
		    }
		  break;
	       default:
		  break;
	       }
	  }
	
	if ((get_time() - t1) > 0.04) 
	  {
	     double shift;
	     double dx, dy;
             double ex, ey, ew, eh;	     

             etox_get_geometry(e_etox, &ex, &ey, &ew, &eh);

	     shift = floor((get_time() - t1) / 0.04);

	     if (!(((ex - etox_x) == 0) && (ey - etox_y) == 0))
	       {
		  dx = (shift < fabs(ex - etox_x)) ? 
		       -(shift * (fabs(ex - etox_x)/(ex - etox_x))) : 
	               -(ex - etox_x); 
		  dy = (shift < fabs(ey - etox_y)) ? 
		       -(shift * (fabs(ey - etox_y)/(ey - etox_y))) : 
	               -(ey - etox_y); 
		  if (dx) 
		    {
		       double ox, oy, ow, oh;
		       int j;
		       etox_move(e_etox, ex+dx, ey);
		       
		       for (j=0; j<NRECTS; j++)
			 {
			    evas_get_geometry(e, rect[j], &ox, &oy, &ow, &oh);
			    evas_move(e, rect[j], ox+dx, oy+dy);
			 }
		    }
		  if (dy)
		    {
		       double ox, oy, ow, oh;
		       int j;
		       etox_move(e_etox, ex, ey+dy);
		       
		       for (j=0; j<NRECTS; j++)
			 {
			    evas_get_geometry(e, rect[j], &ox, &oy, &ow, &oh);
			    evas_move(e, rect[j], ox+dx, oy+dy);
			 }
		    }
	       }
	     if (alpha_up_goal < etox_get_alpha(et_up_label)) {
		etox_set_alpha(et_up_label, (etox_get_alpha(et_up_label) - (shift * 10)) < 0 ? 0 : etox_get_alpha(et_up_label) - (shift * 10));
	     }	     
	     if (alpha_up_goal > etox_get_alpha(et_up_label)) {
		etox_set_alpha(et_up_label, etox_get_alpha(et_up_label) + (shift * 10));
		if (etox_get_alpha(et_up_label) >= 255)
		  alpha_up_goal = 0;
	     }
	     if (alpha_down_goal < etox_get_alpha(et_down_label)) {
		etox_set_alpha(et_down_label, (etox_get_alpha(et_down_label) - (shift * 10)) < 0 ? 0 : etox_get_alpha(et_down_label) - (shift * 10));
	     }	     
	     if (alpha_down_goal > etox_get_alpha(et_down_label)) {
		etox_set_alpha(et_down_label, etox_get_alpha(et_down_label) + (shift * 10));
		if (etox_get_alpha(et_down_label) >= 255)
		  alpha_down_goal = 0;
	     }
	     t1 = get_time();
	  }
	evas_render(e);
     }
}
   
