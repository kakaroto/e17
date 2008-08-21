#include "bg.h"


/*
 * temp global variable hack for trans
 */
Evas *g_evas;
int g_x;
int g_y;
int g_w; 
int g_h;



/*****************************************************************************
 The following function is a slighlty modified form of code taken from evidence, an excellent filemanager by Azundris (evidence.sf.net) 
******************************************************************************/

Evas_Object *transparency_get_pixmap(Evas *evas, Evas_Object *old,
				     int x, int y, int w, int h)
{
   Atom            prop,type; 
   int             format;
   unsigned long   length,after;
   unsigned char  *data;
   Evas_Object    *new=NULL;
   Pixmap          p;
   
   if(old)
     evas_object_del(old);
   
   if((prop=XInternAtom(ecore_x_display_get(),"_XROOTPMAP_ID",True))!=None)
     {
	int ret=XGetWindowProperty(ecore_x_display_get(), 
				   RootWindow(ecore_x_display_get(), 0),
				   prop, 0L, 1L, False, AnyPropertyType, &type,
				   &format,&length, &after,&data);
	if((ret==Success)&&(type==XA_PIXMAP)&&((p=*((Pixmap *)data))!=None)) {
	   Imlib_Image  im;
	   unsigned int pw,ph, pb,pd;
	   int          px,py;
	   Window       win_dummy;
	   Status       st;
	   
	   st=XGetGeometry(ecore_x_display_get(),p,&win_dummy, &px,&py,&pw,&ph, &pb, &pd);
	   if(st&&(pw>0)&&(ph>0)) {
#ifdef NOIR_DEBUG
	      fprintf(stderr,"bg_ebg_trans: transparency update %3d,%3d %3dx%3d\n",x,y,w,h);
#endif
	      
	      imlib_context_set_display(ecore_x_display_get());
	      imlib_context_set_visual(DefaultVisual(ecore_x_display_get(),DefaultScreen(ecore_x_display_get())));
	      imlib_context_set_colormap(DefaultColormap(ecore_x_display_get(),DefaultScreen(ecore_x_display_get())));
	      imlib_context_set_drawable(*((Pixmap *)data));
	      
	      im=imlib_create_image_from_drawable(0,x,y,w,h,1);
	      imlib_context_set_image(im);
	      imlib_image_set_format("argb");
	      new=evas_object_image_add(evas);
	      evas_object_image_alpha_set(new,0);
	      evas_object_image_size_set(new,w,h);   /* thanks rephorm */
	      evas_object_image_data_copy_set(new,imlib_image_get_data_for_reading_only());
	      imlib_free_image();
	      
	      evas_object_image_fill_set(new,0,0,w,h);
	      evas_object_resize(new,w,h);
	      evas_object_move(new,0,0);
	      evas_object_layer_set(new,-9999);
	      evas_object_image_data_update_add(new,0,0,w,h);
	      evas_object_show(new); }
#if 1
	   else  /* this can happen with e16 */
	     fprintf(stderr,"bg_ebg_trans: got invalid pixmap from root-window, ignoring...\n");
#endif
	}
	else
	  fprintf(stderr,"bg_ebg_trans: could not read root-window property _XROOTPMAP_ID...\n"); }
   else
     fprintf(stderr,"bg_ebg_trans: could not get XAtom _XROOTPMAP_ID...\n");
   
   if(!new) {  /* fallback if no root pixmap is found */
#if 1
      fprintf(stderr,"bg_ebg_trans: cannot create transparency pixmap, no valid wallpaper set.\n");
#endif
      new=evas_object_rectangle_add(evas);
      evas_object_resize(new,w,h);
      evas_object_move(new,0,0);
      evas_object_layer_set(new,-9999);
      evas_object_color_set(new, 127,127,127, 255);
      evas_object_show(new); }   
   
   
   return new; 
}

Evas_Object *transparency_get_pixmap_with_windows(Evas *evas, Evas_Object *old, int x, int y, int w, int h)
{	
   Imlib_Image im;
   Evas_Object *ob;
   
   if(old)
     evas_object_del(old);   
   imlib_context_set_display(ecore_x_display_get());
   imlib_context_set_drawable(RootWindow(ecore_x_display_get(),0));
   imlib_context_set_visual( DefaultVisual(ecore_x_display_get(), DefaultScreen(ecore_x_display_get() ) ));
   im = imlib_create_image_from_drawable(0, x,y,w,h,0);
   imlib_context_set_image(im);	     	     
   imlib_image_set_format("argb");
   ob = evas_object_image_add(evas);
   evas_object_image_size_set(ob, w, h);
   evas_object_image_data_copy_set(ob, imlib_image_get_data_for_reading_only());
   evas_object_image_fill_set(ob, 0, 0,  w, h);
   evas_object_resize(ob, w, h);	
   evas_object_image_alpha_set(ob, 0);
   imlib_free_image();
   return ob;
}
