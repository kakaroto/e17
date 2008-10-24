#ifndef _ECLIPSE_H
#define _ECLIPSE_H
#include "eclipse.h"
#endif


typedef struct _Eclipse_Fader Eclipse_Fader;

struct _Eclipse_Fader
{
   Eclipse_View  *view;
   Eclipse_Image *old;
   Eclipse_Image *image;
};


/* show an image */
//void show_image(Ecore_Evas *ee, Evas *evas,Eclipse_Image  *image, Eclipse_Image  *old, char *filename);
void show_image(Eclipse_View*);    
  
/* change transparency of image using wheel */
void image_mouse_wheel_trans(void*, Evas*, Evas_Object*, Evas_Event_Mouse_Wheel*);

/* capture mouse press */
void image_mouse_button_down(void*, Evas*, Evas_Object*, Evas_Event_Mouse_Down*);

/* capture mouse release */
void image_mouse_button_up(void*, Evas*, Evas_Object*, Evas_Event_Mouse_Up*);

/* capture and process key press */
void image_key_capture(void *data, Evas * e, Evas_Object * obj, void *ev);
  
/* fade in / out two images.*/
int image_fadein_fadeout(void* old);  

/* go into slide show mode */
int __mode_slideshow(Eclipse_View *view);
Ecore_Timer *mode_slideshow(Eclipse_View *view);
  
/* go into webcam mode */
int __mode_webcam(Eclipse_View *view);
Ecore_Timer *mode_webcam(Eclipse_View *view);

void get_image_curl(char outfilename[], char* img);
    
Eclipse_Image *eclips_image_new(int layer, Eina_List *list);
    
int evas_object_fade_in(Evas_Object *ob);

int evas_object_fade_out(Evas_Object *ob);
