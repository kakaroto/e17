/* standard headers */
#include <stdio.h>
#include <string.h>
#include <Evas.h>
#include <Ecore.h>
#include <Ecore_X.h>
#include <Ecore_Evas.h>
#include <Edje.h>
#include <X11/Xlib.h>
#include <Imlib2.h>
#include <math.h>
#include "eclipse.h"
#include "images.h"
#include "bg.h"
#include "window.h"
#include "keys.h"

/* variables */
Ecore_Evas  *ee;           /* our ecore_evas */
Evas        *evas;         /* our evas       */
Evas_Object *ob,*bg;       /* our background */
Eina_List   *l,*l2,*views; /* our view list  */

Eclipse_Image   *image, *picnpic;
Eclipse_Options *eo;
Eclipse_Options *options;

int iw, ih;
  
/* main program */
int main(int argc, char **argv)
{
   
   /* required variables */
   Ecore_Timer  *timer;
   
   /* make sure we have an image or image list */
   if(argc<2)usage();

   /* parse the input we get from the    
    * user and set default options first 
    */
   ecore_evas_init();
   ee = ecore_evas_software_x11_new(0, 0, 0, 0, 0, 0);
   evas =  ecore_evas_get(ee);   
   eo = setup_window(ee,eo,argc,argv);
   
   /* handle desktop translucency if activated */
   bg = draw_background(eo,ee,evas);
   
   
   /* go through available views and display them properly */
   while(views)
     {
	switch(((Eclipse_View*)eina_list_data_get(views))->mode)
	  {
	   case 1:
	     /* pnp mode */
	       {		  
		  Eclipse_View *view = (Eclipse_View*)eina_list_data_get(views);
		  view->curimg = NULL;
		  view->layer  = 6;
		  view->x = 10;
		  view->y = 10;
		  view->w = 80;
		  view->h = 0;
		  show_image(view);
		  if(strcmp(eo->mode,"s") == 0)
		    timer = mode_slideshow(view);
		  else if(strcmp(eo->mode,"w") == 0)
		    timer = mode_webcam(view);
	       }	     	     
	     break;
	   case 2:
	     /* multi mode */
	     printf("entering multi view\n");
	       {
		  /* count number of view ports we need */
		  Eina_List *l = views;
		  int viewports = 0,i,j;
		  while(l)
		    {
		       if(((Eclipse_View*)eina_list_data_get(views))->mode == 2)
			 viewports++;
		       l = eina_list_next(l);
		    }
		  eo->geom_h = eo->geom_w = (90+10+10)*(int)(ceil(sqrt((double)viewports)));
		  /* for now, we want to draw a square, TODO: user input */
		  l = views;
		  for(i=0;i<(int)(ceil(sqrt((double)viewports))); i++)
		    {
		       for(j=0;j<(int)(ceil(sqrt((double)viewports))); j++)
			 {
			    int go = 1;
			    while(go == 1&&l)
			      {
				 if(((Eclipse_View*)eina_list_data_get(l))->mode == 2)
				   {
				      Eclipse_View *view = (Eclipse_View*)eina_list_data_get(l);
				      view->curimg = NULL;
				      view->layer  = 5;
				      view->x = 0+j*90+10+j*10;
				      view->y = 0+i*90+10+i*10;
				      view->w = 90;
				      view->h = 90;
				      show_image(view);
				      view->mode = -2;
				      if(strcmp(eo->mode,"s") == 0)
					timer = mode_slideshow(view);
				      else if(strcmp(eo->mode,"w") == 0)
					timer = mode_webcam(view);
				      go = -1;
				      l = eina_list_next(l);
				      break;
				   }				 
			      }			    
			 }		       
		    }		  
	       }	     
	     break;
	   case 3:
	     /* pcn mode - not working yet */
	     break;
	   case 0:
	     /* default mode */
	       {
		  Eclipse_View *view = (Eclipse_View*)eina_list_data_get(views);		  
		  show_image(view);
		  if(strcmp(eo->mode,"s") == 0)
		    timer = mode_slideshow(view);
		  else if(strcmp(eo->mode,"w") == 0)
		    timer = mode_webcam(view);
	       }	     
	     break;
	     
	  }
	views = eina_list_next(views);
     }   
   /* show main window */
   ecore_evas_show(ee);
             
   /* start the event loop */
   ecore_main_loop_begin();   
   
   /* shutdown the subsystems */
   ecore_evas_shutdown();
   ecore_shutdown();
            
   return 0;   
}
