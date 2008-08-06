/* ETK - The Enlightened ToolKit
 * Copyright (C) 2006-2008 Simon Treny, Hisham Mardam-Bey, Vincent Torri, Viktor Kojouharov
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. 
 * If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _ETK_ENGINE_ECORE_EVAS_FB_H_
#define _ETK_ENGINE_ECORE_EVAS_FB_H_

#include <Evas.h>

#define ETK_ENGINE_ECORE_FB_WINDOW_DATA(data) ((Etk_Engine_Ecore_Fb_Window_Data*)data)

/* Engine specific data for Etk_Window */
typedef struct Etk_Engine_Ecore_Fb_Window_Data
{
   Etk_Position border_position;
   Etk_Size size;
   Etk_Size min_size;

   char *title;
   Evas_Object *border;
   Etk_Bool borderless:1;
   Etk_Bool visible:1;
   Etk_Bool maximized:1;
} Etk_Engine_Ecore_Fb_Window_Data;

#endif
