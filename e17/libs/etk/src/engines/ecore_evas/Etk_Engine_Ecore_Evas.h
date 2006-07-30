#ifndef _ETK_ENGINE_ECORE_EVAS_H
#define _ETK_ENGINE_ECORE_EVAS_H

#include <Ecore_Evas.h>

#define ETK_ENGINE_ECORE_EVAS_WINDOW_DATA(data) ((Etk_Engine_Ecore_Evas_Window_Data*)data)

/* Engine specific data for Etk_Window */
typedef struct Etk_Engine_Ecore_Evas_Window_Data
{
   Ecore_Evas *ecore_evas;
} Etk_Engine_Ecore_Evas_Window_Data;

#endif
