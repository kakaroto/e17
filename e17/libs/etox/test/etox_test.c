#include "Evas.h"
#include "src/Etox.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <math.h>

#define IMGDIR "./img/"
#define FNTDIR "./fnt"

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
   Evas_Object o[3];

   E_Font_Style  *e_font_style;
   Etox_Bit      *e_etox_bit;
   Etox          *e_etox;
   Etox          *e_etox_2;
   XSetWindowAttributes att;
   Evas_Render_Method method;
   
   int down;
   double t1, t2;
   double etox_x, etox_y;
   
   Display *disp;
   Window   win;
   Window   root_win;
   Visual  *vis;
   Colormap cm;
   int      depth;
   
   char txt[4096];
   
   win_w = 640; win_h = 480;
   /* the program... */

   /* connect to X */
   disp  = XOpenDisplay(NULL);
   /* get default visual , colormap etc. you could ask imlib2 for what it */
   /* thinks is the best, but this example is intended to be simple */
   root_win = DefaultRootWindow(disp);

   e = evas_new_all(disp, root_win, 128, 0, win_w, win_h,
		    0, 216, 4 * 1024 * 1024, 4 * 1024 * 1024,
		    FNTDIR);

   win = evas_get_window(e);
   
   /* tell X what events we are interested in */
   XSelectInput(disp, win, ButtonPressMask | ButtonReleaseMask | 
                PointerMotionMask | ExposureMask);
   XMapWindow(disp, win);
//   XSync(disp, False);

   vis = evas_get_optimal_visual(e, disp);
   cm = evas_get_optimal_colormap(e, disp);
   depth = evas_get_colors(e);
   
   o[0] = evas_add_image_from_file(e, IMGDIR"sky001.png");
   evas_lower(e, o[0]);
   evas_show(e, o[0]);
   
   o[1] = evas_add_image_from_file(e, IMGDIR"arrow_up.png");
   evas_move(e, o[1], 500, 10);
   evas_show(e, o[1]);
   
   o[2] = evas_add_image_from_file(e, IMGDIR"arrow_down.png");
   evas_move(e, o[2], 500, 400);
   evas_show(e, o[2]);
   
   e_font_style = E_load_font_style("sh_ol.style");
   
   e_etox = Etox_new("Etox name");
   e_etox->evas = e;
   etox_set_font_style(e_etox, e_font_style);
   etox_set_layer(e_etox, 10);
   e_etox->font = strdup("cinema");
   e_etox->font_size = 10;
   e_etox->w=450;
   e_etox->h=1000;

   strcpy(txt,"~color=fg 255 2 2~~color=ol 0 0 0~~color=sh 0 0 0~~valign=bottom~~font=morpheus~~size=20~");
   strcat(txt,"~align=center~The Gospel of Tux (v1.0)\n\n");

   strcat(txt,"~font=notepad~~size=12~~color=fg 255 255 255~~align~In the beginning Turing created the Machine.\n\n");

   strcat(txt,"~align=right~And the Machine was crufty and bodacious, existing in ");
   strcat(txt,"theory only. And von Neumann looked upon the Machine, ");
   strcat(txt,"and saw that it was crufty. He divided the Machine ");
   strcat(txt,"into two Abstractions, the Data and the Code, and yet ");
   strcat(txt,"the two were one Architecture. This is a great ");
   strcat(txt,"Mystery, and the beginning of wisdom.\n\n");

   strcat(txt,"~align~~style=plain.style~And von Neumann spoke unto the Architecture, and ");
   strcat(txt,"blessed it, saying, \"Go forth and replicate, freely ");
   strcat(txt,"exchanging data and code, and bring forth all manner ");
   strcat(txt,"of devices unto the earth.\" And it was so, and it was ");
   strcat(txt,"cool. The Architecture prospered and was implemented ");
   strcat(txt,"in hardware and software. And it brought forth many ");
   strcat(txt,"Systems unto the earth.\n\n");
     

   strcat(txt,"~style~The first Systems were mighty giants; many great ");
   strcat(txt,"works of renown did they accomplish. Among them were ");
   strcat(txt,"Colossus, the codebreaker; ENIAC, the targeter; EDSAC ");
   strcat(txt,"and MULTIVAC and all manner of froody creatures ");
   strcat(txt,"ending in AC, the experimenters; and SAGE, the ");
   strcat(txt,"defender of the sky and father of all networks. These ");
   strcat(txt,"were the mighty giants of old, the first children of ");
   strcat(txt,"Turing, and their works are written in the Books of ");
   strcat(txt,"the Ancients. This was the First Age, the age of ");
   strcat(txt,"Lore.\n\n");

   strcat(txt,"Now the sons of Marketing looked upon the children of ");
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
   
   etox_set_text(e_etox, txt);
   etox_x = 5; etox_y = 0;
   etox_move(e_etox, etox_x, etox_y);
   etox_show(e_etox);

   e_etox_2 = Etox_new("Showoff");
   e_etox_2->evas = e;
   etox_set_font_style(e_etox_2, e_font_style);
   etox_set_layer(e_etox_2, 10);
   e_etox_2->font = strdup("cinema");
   e_etox_2->font_size = 10;
   e_etox_2->w=210;
   e_etox_2->h=1000;
   
   strcpy(txt,"~color=fg 255 255 255~~color=ol 0 0 0~~color=sh 0 0 0~~valign=bottom~~font=notepad~~size=14~Various vertical and horizontal alignments:\n");
   strcat(txt,"~color=fg 255 2 2~~color=ol 0 0 0~~color=sh 0 0 0~~valign=bottom~~font=morpheus~");
   strcat(txt,"~valign=top~~align=left~~size=25~A ~size=23~A ~size=21~A ~size=19~A ~size=17~A\n");
   strcat(txt,"~valign=center~~align=center~~size=25~A ~size=23~A ~size=21~A ~size=19~A ~size=17~A\n");
   strcat(txt,"~valign=bottom~~align=right~~size=25~A ~size=23~A ~size=21~A ~size=19~A ~size=17~A\n");
   
   etox_set_text(e_etox_2, txt);
   etox_move(e_etox_2,400,45);
   etox_show(e_etox_2);
  
   evas_move(e, o[0], 0, 0);
   evas_resize(e, o[0], win_w, win_h);
   evas_set_image_fill(e, o[0], 0, 0, win_w, win_h);

   t1 = get_time();
   for (;;)
     {
	double x, y;
	XEvent              ev;


	while (XPending(disp))
	  {
	     XNextEvent(disp, &ev);
	     switch(ev.type)
	       {
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
			      }
			    else if (obj == o[2])
			      {
				 etox_y -= 25;
			      }
			 }
		       if (button == 3)
			 {
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
	     
	     shift = floor((get_time() - t1) / 0.04);

	     if (!(((e_etox->x - etox_x) == 0) && (e_etox->y - etox_y) == 0))
	       {
		  dx = (shift < fabs(e_etox->x - etox_x)) ? 
		       -(shift * (fabs(e_etox->x - etox_x)/(e_etox->x - etox_x))) : 
	               -(e_etox->x - etox_x); 
		  dy = (shift < fabs(e_etox->y - etox_y)) ? 
		       -(shift * (fabs(e_etox->y - etox_y)/(e_etox->y - etox_y))) : 
	               -(e_etox->y - etox_y); 
		  if (dx)
		    etox_move(e_etox, e_etox->x+dx, e_etox->y);
		  if (dy)
		    etox_move(e_etox, e_etox->x, e_etox->y+dy);
	       }
	     t1 = get_time();
	  }
	evas_render(e);
     }
}
   
