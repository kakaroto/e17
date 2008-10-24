#include "eclipse.h"
#include <Evas.h>
#include <unistd.h>
#include <math.h>
#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Ecore_X.h>
#include <limits.h>
#ifdef HAVE_CURL_CURL_H
#include <curl/curl.h>
#endif
#include "images.h"
#include "keys.h"
#include "menu.h"

/* vars we need to get from ecplipse.c */
extern Eclipse_Image   *image;
extern Ecore_Evas      *ee;
extern Evas            *evas;
extern Eclipse_Options *eo;
extern char            *pressed_key;
Ecore_Timer            *timer;
int                    t=0;
int r,r2,g,g2,b,b2,a,a2=255,a_old,display=1;

/* show an image */
void
  show_image(Eclipse_View *view)
{
   Evas_Object   *ob;
   Eclipse_Image *image;
   
   int    iw,ih,layer;
   char  *filename;
   float  f;   
   
   view->t=0;
   
   /* if old image exists, get its transparency level */
   if(view->curimg)
	evas_object_color_get(view->curimg->img,&r,&g,&b,&a);        
  
   ob = evas_object_image_add(evas);
   image = eclips_image_new(0,NULL); /* FIXME */
   image->img = ob;
   
   /* we need to see if the file is local or not */
   filename = strdup((char*)eina_list_data_get(view->images));
   if ((!strncmp(filename, "http://", 7)) ||
       (!strncmp(filename, "https://", 8)) ||
       (!strncmp(filename, "ftp://", 6)))
     {
	/* we have two options, either use curl if its installed, or built in agent */
	  {	     
	     char outfilename[PATH_MAX]; 
	     snprintf(outfilename, PATH_MAX, "/tmp/eclips%d%s",getpid(),get_shortname_for(filename));
             #ifdef HAVE_LIBCURL
	     get_image_curl(outfilename,filename);
             #else	
	     printf("no code here!!\n");
             #endif
	     evas_object_image_file_set(ob, outfilename, NULL);
	  }	
     }   
   else
     evas_object_image_file_set(ob, filename, NULL);
   free(filename);
   
   evas_object_image_alpha_set(ob,1);
   evas_object_color_get(ob,&r2,&g2,&b2,&a2);
   evas_object_color_set(ob,r2,g2,b2,0);
   evas_object_image_size_get(ob, &iw, &ih);

   
   if(view->w != 0 && view->h == 0)
     {
	iw = view->w;
	  {	     
	     int w,h;
	     evas_object_image_size_get(ob, &w, &h);
	     ih = (int)((80.0/(float)w)*(float)h);
	  }		
	evas_object_move(image->img,view->x,view->y);
     }
   else if(view->w != 0 && view->h != 0)
     {	
	iw = view->w;
	ih = view->h;
	if(view->x >= 0 && view->y >= 0)
	  {
	     evas_object_move(image->img,view->x,view->y);
	  }
     }
   else 
     {	
	/* check to see if we have fixed window geometry */
	if(eo->geom_h != 0 && eo->geom_w != 0)
	  {
	     
	     if(eo->geom_h >= ih && eo->geom_w <= iw)
	       {
		  /* scale height according to percentage */
		  f = (float)iw/(float)eo->geom_w;
		  ih = (int)(1/(float)f*(float)ih);
		  /* make image width = window width */
		  iw = eo->geom_w;	     
	       }
	     else if(eo->geom_h <= ih && eo->geom_w >= iw)
	       {
		  /* scale width according to percentage */
		  f = (float)ih/(float)eo->geom_h;
		  iw = (int)(1/(float)f*(float)iw);
		  /* make image height = window height */
		  ih = eo->geom_h;	     
	       }	
	     /* if window size < image size */
	     else if(eo->geom_h <= ih && eo->geom_w <= iw)
	       {
		  if(iw >= ih)
		    {	
		       if(eo->geom_w >= eo->geom_h)
			 {
			    f = (float)iw/(float)eo->geom_w;
			    ih = (int)(1/(float)f*(float)ih);
			    iw = eo->geom_w;		       		       
			 }
		       else
			 {
			    f = (float)ih/(float)eo->geom_h;
			    iw = (int)(1/(float)f*(float)iw);		       
			    ih = eo->geom_h;
			 }
		    }
		  else
		    {
		       if(eo->geom_w >= eo->geom_h)
			 {
			    f = (float)ih/(float)eo->geom_h;
			    iw = (int)(1/(float)f*(float)iw);
			    ih = eo->geom_h;
			 }
		       else
			 {
			    f = (float)iw/(float)eo->geom_w;
			    ih = (int)(1/(float)f*(float)ih);
			    iw = eo->geom_w;
			 }
		    }
	       }
	     /* if window size > image size */
	     else if(eo->geom_h > ih && eo->geom_w > iw)
	       {
		  if(iw >= ih)
		    {	
		       if(eo->geom_w >= eo->geom_h)
			 {		       
			    f = (float)ih/(float)eo->geom_h;
			    iw = (int)(1/(float)f*(float)iw);		       
			    ih = eo->geom_h;
			 }
		       else
			 {
			    f = (float)iw/(float)eo->geom_w;
			    ih = (int)(1/(float)f*(float)ih);
			    iw = eo->geom_w;
			 }
		    }
		  else
		    {
		       if(eo->geom_w >= eo->geom_h)
			 {		       
			    f = (float)iw/(float)eo->geom_w;
			    ih = (int)(1/(float)f*(float)ih);
			    iw = eo->geom_w;
			 }
		       else
			 {
			    f = (float)ih/(float)eo->geom_h;
			    iw = (int)(1/(float)f*(float)iw);
			    ih = eo->geom_h;
			 }
		    }
	       }	  
	     
	     evas_object_move(ob,(eo->geom_w-iw)/2,(eo->geom_h-ih)/2);
	  }  
	else
	  {
	     eo->geom_w = iw;
	     eo->geom_h = ih;
	  }
     }   
   
   evas_object_resize(ob, iw, ih);
   evas_object_image_fill_set(ob, 0, 0, iw, ih);
   
   /* <temp hack for shadow> */
   if(eo->shadows == 1)
     {	
	Evas_Coord wi,hi,xi,yi;	
	int ws,hs,xs,ys;
	float wp,hp,kh=1.0,kw=1.0;	
	if(view->shadow)evas_object_del(view->shadow);
	view->shadow = evas_object_image_add(evas);
	evas_object_image_file_set(view->shadow, DATADIR"shadow.png",NULL);
	evas_object_geometry_get(ob,&xi,&yi,&wi,&hi);		  
	evas_object_image_size_get(view->shadow,&ws,&hs);
	wp = (float)ws/(float)wi;if(wp>1.0)wp = 1.0/wp;
	hp = (float)hs/(float)hi;if(hp>1.0)hp = 1.0/hp;
	ws = wi+(int)(32.0*2.0*wp*kw);
	hs = hi+(int)(32.0*2.0*hp*kh);				
	xs = xi - (int)(32.0*wp*kw);
	ys = yi - (int)(32.0*hp*kh);
//	xs = (int)(xi)-(32);
//	ys = (int)(yi)-(32);
//	ws = (int)(wi)+(64);
//	hs = (int)(hi)+(64);
	evas_object_move(view->shadow,xs,ys);
	evas_object_resize(view->shadow, ws, hs);
	evas_object_image_fill_set(view->shadow, 0,0,ws,hs);
	evas_object_color_set(view->shadow,0,0,0,200);
	evas_object_layer_set(view->shadow,view->layer-1);
	evas_object_show(view->shadow);
	
     }   
   /* </temp hack for shadow> */
   
   if (view->layer < 1) layer = 2;
   else layer = view->layer;
   
   evas_object_layer_set(ob, layer);
   evas_object_show(ob);
   ecore_evas_resize(ee,eo->geom_w,eo->geom_h);
   evas_object_event_callback_add(ob, EVAS_CALLBACK_KEY_DOWN, (void*)cb_key_down, NULL);
   evas_object_event_callback_add(ob, EVAS_CALLBACK_KEY_UP, (void*)cb_key_up, NULL);   
   evas_object_event_callback_add(ob, EVAS_CALLBACK_MOUSE_WHEEL, (void*)image_mouse_wheel_trans,(void*)view);
   evas_object_event_callback_add(ob, EVAS_CALLBACK_MOUSE_DOWN, (void*)image_mouse_button_down,(void*)view);
   evas_object_event_callback_add(ob, EVAS_CALLBACK_MOUSE_UP, (void*)image_mouse_button_up,NULL);
   evas_object_focus_set(ob,1);
   
   if(view->curimg)
     {
	/* set a timer for the fading process */
	printf("show_image() Going into fade in/out.\n");
	view->a_old = a;
	  {
	     Eclipse_Fader *opt = (Eclipse_Fader*)calloc(1,sizeof(Eclipse_Fader));
	     opt->old = view->curimg;
	     opt->image = image;
	     opt->view = view;
	     timer = ecore_timer_add(eo->transition_timer, image_fadein_fadeout, (void*)opt);
	  }	
     }
   else
     /* no fading needed, just display */
     evas_object_color_set(ob,r2,g2,b2,eo->trans);
   view->curimg = image;
}

/* fade in / out two images */
int 
  image_fadein_fadeout(void *o)
{   
   int a,r,g,b,a2,r2,g2,b2;
   Eclipse_Fader *opt = (Eclipse_Fader*)o;
   opt->view->display = 0;      
   evas_object_color_get(opt->image->img,&r2,&g2,&b2,&a2);
   evas_object_color_get(opt->old->img,&r,&g,&b,&a);
   
   evas_object_color_set(opt->image->img,r2,g2,b2,opt->view->t);
   evas_object_color_set(opt->old->img,r,g,b,(int)(a-(0.1)*opt->view->t));
   
   opt->view->t+=eo->alpha_increment;
   if(opt->view->t>opt->view->a_old)
     {	
	evas_object_color_set(opt->image->img,r2,g2,b2,opt->view->a_old);
	evas_object_del(opt->old->img);
	opt->view->display = 1;
	printf("image_fadein_fadeout() Finished fade in/out.\n");
	return 0;
     }   
   return 1;
}


/* change transparency of image using wheel */
void 
  image_mouse_wheel_trans(void *data, 
			  Evas * e, 
			  Evas_Object * obj, 
			  Evas_Event_Mouse_Wheel * ev)
{   
   int r,g,b,a;
   Eclipse_View *view;
   view = (Eclipse_View*)data;
   evas_object_color_get(view->curimg->img,&r,&g,&b,&a);
   evas_object_color_set(view->curimg->img,r,g,b,a+5*ev->z);
   if(view->shadow)evas_object_color_set(view->shadow,r,g,b,a+5*ev->z-55);
   printf("image_mouse_wheel_trans() Changing transparency by %d (%.0f%%)\n",5*ev->z,(float)((a+5*ev->z)*(100)/250));
}

/* handle mouse button press */
void 
  image_mouse_button_down(void *data, 
			     Evas * e, 
			     Evas_Object * obj, 
			     Evas_Event_Mouse_Down * ev)
{   
   char name[256];   
   Eclipse_Image *img = data;
   Eclipse_Image *old = (Eclipse_Image*)calloc(1,sizeof(Eclipse_Image));
   Eclipse_View *view = (Eclipse_View*)data;
   old->img = obj;
   printf("image_mouse_button_down() Pressed mouse button %d\n",ev->button);
   if(view->display==0)
     {	
	printf("image_mouse_button_down() Waiting for fade in/out to finish.\n");
	return;
     }   
   
   /* cycle to next image */
   if(ev->button == 1)
     {
	if(eo->random == 1)
	  {
	     int r;
	     srand((int)ecore_time_get());
	     r = 0+(int) ((float)(eina_list_count(view->imfirst))*rand()/(INT_MAX+1.0));
	     printf("using image no. %d from %d\n",r,eina_list_count(view->imfirst));
	     view->images = eina_list_nth_list(view->imfirst,r);
	  }
	else
	  {
	     
	     if(!view->images->next)
	       while(view->images->prev)view->images=eina_list_prev(view->images);
	     else
	       view->images = eina_list_next(view->images);
	  }	
	show_image(view);
     }   
   /* cycle to previous image */
   else if(ev->button == 3)
     {
	
	if(eo->random == 1)
	  {
	     int r;
	     srand((int)ecore_time_get());
	     r = 0+(int) ((float)(eina_list_count(view->imfirst))*rand()/(INT_MAX+1.0));
	     printf("using image no. %d from %d\n",r,eina_list_count(view->imfirst));
	     view->images = eina_list_nth_list(view->imfirst,r);
	  }
	else
	  {	
	     if(!view->images->prev)
	       view->images = eina_list_last(view->images);
	     else
	       view->images = eina_list_prev(view->images);
	  }	
	show_image(view);
     }   
   /* show menu */
   else if(ev->button == 2)
     {
	if(pressed_key != NULL)
	  {	     
	     if(!strcmp(pressed_key,"Control_L")||!strcmp(pressed_key,"Control_R"))
	       show_image_info_menu(ev,view);
	     else
	       root_menu_show(ev,view);
	  }	
	else
	  root_menu_show(ev,view);
     }   
}

/* handle mouse button release */
void 
  image_mouse_button_up(void *data, 
			Evas * e, 
			Evas_Object * obj, 
			Evas_Event_Mouse_Up * ev)
{
   if(ev->button == 2)
     {
	root_menu_hide();
     }   
}


Ecore_Timer
  *mode_slideshow(Eclipse_View *view)
{
   return ecore_timer_add((int)((float)eo->delay+(float)eo->transition_timer*(float)255/(float)eo->alpha_increment),
			   (void*)__mode_slideshow,
			   (void*)view);
}

Ecore_Timer
  *mode_webcam(Eclipse_View *view)
{
   return ecore_timer_add((int)((float)eo->delay+(float)eo->transition_timer*(float)255/(float)eo->alpha_increment),
			   (void*)__mode_webcam,
			   (void*)view);
}

int 
  __mode_slideshow(Eclipse_View *view)
{
   if(!view)
     {
	printf("FATAL: mode_slideshow(): view parameter undefined!\n");
	return 0;
     }
   
   /* we dont want to display images prematurely, we'll lose alpha settings */
   if(view->display==0) // <=----------- use this for middle click too
     {
	printf("mode_slideshow() Waiting for fade in/out to finish.\n");
	return 1;
     }
   
   printf("mode_slideshow() Continuing slide show.\n");
   if(eo->random == 1)
     {
	int r;
	srand((int)ecore_time_get());
	r = 0+(int) ((float)(eina_list_count(view->imfirst))*rand()/(INT_MAX+1.0));
	printf("using image no. %d from %d\n",r,eina_list_count(view->imfirst));
	view->images = eina_list_nth_list(view->imfirst,r);
     }
   else 
     {	
	if(!view->images->next)
	  while(view->images->prev)view->images=eina_list_prev(view->images);
	else
	  view->images = eina_list_next(view->images);
     }   
   show_image(view);
   return 1;
}

int 
  __mode_webcam(Eclipse_View *view)
{     
   show_image(view);
   return 1;
}

void
  get_image_curl(char outfilename[], char *img)
{
   CURL *curl;
   CURLcode res;
   FILE *outfile;
   curl = curl_easy_init();
   if(curl)
     {		  
	outfile = fopen(outfilename, "w");
	curl_easy_setopt(curl, CURLOPT_FILE, outfile);
	curl_easy_setopt(curl, CURLOPT_URL, img);
	res = curl_easy_perform(curl);
	fclose(outfile);
	curl_easy_cleanup(curl);
     }
}

Eclipse_Image 
  *eclips_image_new(int        layer, 
		    Eina_List *list)
{
   Eclipse_Image *im;
   im = (Eclipse_Image*)calloc(1,sizeof(Eclipse_Image));
   im->x=im->y=im->w=im->h=0;
   im->layer = layer;
   im->list = list;
   return im;
}

/**
 * evas_object_fade_in - Fade in an evas object from a=0
 * @i The Evas_Object we're fading in
 */
int evas_object_fade_in(Evas_Object *ob)
{   
   int r,g,b,a;
   evas_object_color_get(ob, &r, &g, &b, &a);
   if(a < eo->menu_trans)
     a +=20;
   else return 0;
   evas_object_color_set(ob, r, g, b, a);
   return 1;
}

/**
 * evas_object_fade_out - Fade out an evas object to a=0
 * @i The Evas_Object we're fading out
 */
int evas_object_fade_out(Evas_Object *ob)
{
   int r,g,b,a;
   evas_object_color_get(ob, &r, &g, &b, &a);
   if(a > 0)
     a -=20;
   else
     {
	evas_object_color_set(ob, r, g, b, 0);
	return 0;	
     }   
   evas_object_color_set(ob, r, g, b, a);
   return 1;
}   
