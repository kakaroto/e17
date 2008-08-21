#include <Evas.h>
#include <unistd.h>
#include <math.h>
#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Ecore_X.h>
#include "window.h"
#include "eclipse.h"
#include "keys.h"
#include "getopt.h"

extern Eclipse_Options *eo;
extern Evas_Object *bg;
extern Evas_Object *ob;
extern int iw,ih;

/**
 * cb_resize - callback for window resizing
 * @ee the Ecore_Evas we're working with
 */
void cb_resize(Ecore_Evas *ee)
{   
   int x, y, w, h;  
   
   ecore_evas_geometry_get(ee, &x, &y, &w, &h);   
   eo->geom_h = h;
   eo->geom_w = w;
   evas_object_resize(bg, w, h);

   if(eo->pseudo_trans != 0)
     {	
	if(eo->pseudo_trans == 1)
	  bg = (Evas_Object*)transparency_get_pixmap(ecore_evas_get(ee), bg, x, y, w, h);
	else
	  {	     
	     bg = (Evas_Object*)transparency_get_pixmap_with_windows(ecore_evas_get(ee), bg,  x, y, w, h);
	     evas_object_show(bg);
	  }	
     }
   else
     {
	evas_object_image_file_set(bg, DATADIR"checkered_bg.png", NULL);
	evas_object_layer_set(bg, 0);
	evas_object_image_size_get(ob, &iw, &ih);	
	evas_object_image_fill_set(bg, 0, 0, iw, ih);
     }
   evas_object_event_callback_add(bg, EVAS_CALLBACK_KEY_DOWN, cb_key_down, NULL);
   evas_object_event_callback_add(bg, EVAS_CALLBACK_KEY_UP, cb_key_up, NULL);
   evas_object_focus_set(bg,1);
}


/**
 * cb_move - callbacl for window movement
 * @ee the Ecore_Evas we're working with
 */
void cb_move(Ecore_Evas *ee)
{
   int x, y, w, h;  
   
   ecore_evas_geometry_get(ee, &x, &y, &w, &h);   
   eo->geom_h = h;
   eo->geom_w = w;   
   evas_object_resize(bg, w, h);

   if(eo->pseudo_trans != 0)
     {	
	if(eo->pseudo_trans == 1)
	  bg = (Evas_Object*)transparency_get_pixmap(ecore_evas_get(ee), bg, x, y, w, h);
	else
	  {
	     bg = (Evas_Object*)transparency_get_pixmap_with_windows(ecore_evas_get(ee), bg,  x, y, w, h);
	     evas_object_show(bg);
	  }       	
     }
   else
     {
	evas_object_image_file_set(bg, DATADIR"checkered_bg.png", NULL);
	evas_object_layer_set(bg, 0);
	evas_object_image_size_get(ob, &iw, &ih);	
	evas_object_image_fill_set(bg, 0, 0, iw, ih);
     }   
   evas_object_event_callback_add(bg, EVAS_CALLBACK_KEY_DOWN, cb_key_down, NULL);
   evas_object_event_callback_add(bg, EVAS_CALLBACK_KEY_UP, cb_key_up, NULL);
   evas_object_focus_set(bg,1);   
}



/**
 * setup_window - Initialize the main window and set app options
 * @ee the Ecore_Evas we're working with
 * @options the application's options variable
 * @argc argc given to the app
 * @argv argv given to the app
 */
Eclipse_Options
  *setup_window(Ecore_Evas      *ee, 
	       Eclipse_Options *options,
	       int              argc, 
	       char           **argv)
{
   /* parse the input we get from the user */
   options = (Eclipse_Options*)calloc(1,sizeof(Eclipse_Options));
   /* set default options first */
   options->transition_timer = 0.01;
   options->alpha_increment  = 10;
   options->delay            = 5;
   options->trans            = 255;
   options->mode             = strdup("n");
   options->geom_w           = 0;
   options->geom_h           = 0;
   options->pseudo_trans     = 0;
   options->menu_trans       = 155;
   options->shadows          = 0;
   options->random           = 0;   
   options = parseArgs(argc,argv,options);

   if (!ee)
     {	
	printf("ERROR: can't create a software X11 Ecore_Evas.\n");
	exit(-1);
     }         
   /* tie move / resize window callback functions */
   ecore_evas_callback_resize_set(ee, cb_resize);
   ecore_evas_callback_move_set(ee, cb_move);
   ecore_evas_title_set(ee,"eclips");
   if(options->borderless == 1)
     {	
	ecore_evas_shaped_set(ee,1);
	ecore_evas_borderless_set(ee,1);
     }
   
   
   return options;
}

/**
 * draw_background - draws the background image on the main window
 * @eo the global app options variable
 * @ee the Ecore_Evas we're dealing with
 * @evas the evas we're working on
 */
Evas_Object 
  *draw_background(Eclipse_Options *eo,Ecore_Evas *ee, Evas *evas)
{
   Evas_Object *bg;
   int          x, y, w, h;
   
   bg = (Evas_Object*)calloc(1,sizeof(Eclipse_Options));
   /* handle desktop translucency if activated */
   if(eo->pseudo_trans != 0)
     {	
	ecore_evas_geometry_get(ee, &x, &y, &w, &h);
	bg = evas_object_image_add(evas);	
	if(eo->pseudo_trans == 1)
	  bg = (Evas_Object*)transparency_get_pixmap(evas, NULL, x, y, w, h);
	else
	  bg = (Evas_Object*)transparency_get_pixmap_with_windows(evas, NULL, x, y, w, h);
	evas_object_layer_set(bg, 0);
	evas_object_show(bg);
     }
   
   else
     {	
	bg = evas_object_image_add(evas);
	evas_object_image_file_set(bg, DATADIR"checkered_bg.png", NULL);
	evas_object_image_fill_set(bg, 0, 0, 0, 0);
	evas_object_layer_set(bg, 0);
	evas_object_show(bg);
     }   
   return bg;
}
