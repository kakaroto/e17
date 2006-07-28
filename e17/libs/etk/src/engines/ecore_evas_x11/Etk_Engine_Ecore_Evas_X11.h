#ifndef _ETK_ENGINE_ECORE_EVAS_X11_H
#define _ETK_ENGINE_ECORE_EVAS_X11_H

#include "Etk_Engine_Ecore_Evas.h"

#define ETK_ENGINE_ECORE_EVAS_X11_WINDOW_DATA(data) ((Etk_Engine_Ecore_Evas_X11_Window_Data*)data)

/* Engine specific data for Etk_Window */
typedef struct Etk_Engine_Ecore_Evas_X11_Window_Data
{
   Etk_Engine_Ecore_Evas_Window_Data data;
   Ecore_X_Window x_window;   
} Etk_Engine_Ecore_Evas_X11_Window_Data;

#endif
