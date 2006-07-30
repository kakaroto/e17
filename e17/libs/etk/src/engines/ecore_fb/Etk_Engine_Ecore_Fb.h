#ifndef _ETK_ENGINE_ECORE_EVAS_FB_H
#define _ETK_ENGINE_ECORE_EVAS_FB_H

#include <Evas.h>

#define ETK_ENGINE_ECORE_FB_WINDOW_DATA(data) ((Etk_Engine_Ecore_Fb_Window_Data*)data)

/* Engine specific data for Etk_Window */
typedef struct Etk_Engine_Ecore_Fb_Window_Data
{
   Etk_Position border_position;
   Etk_Size size;
   Etk_Size min_size;
   Etk_Bool visible;
   
   Evas_Object *border;
} Etk_Engine_Ecore_Fb_Window_Data;

#endif
