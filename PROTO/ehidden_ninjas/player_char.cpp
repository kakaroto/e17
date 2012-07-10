/*
 * Enlightement Hidden Ninjas.
 *
 * Copyright 2012 Hermet (ChunEon Park)
 *
 * This application is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This application is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#include "maths.h"
#include "defines.h"
#include "player_char.h"

using namespace std;
using namespace ehninjas;

PlayerChar ::PlayerChar() :Character(string(""), ID_PLAYER), img(NULL)
{
}



PlayerChar ::~PlayerChar()
{
}



Eina_Bool PlayerChar ::Initialize()
{
   return EINA_TRUE;
}



Eina_Bool PlayerChar ::Release()
{
   return EINA_TRUE;
}



Eina_Bool PlayerChar ::SetImgObj(Evas_Object* img)
{
   this->img = img;
   return EINA_TRUE;
}



const Evas_Object *PlayerChar ::GetImgObj()
{
   return NULL;
}

Eina_Bool PlayerChar ::Move(const VECTOR2& dir, ELEMENT_TYPE velocity)
{
/*   switch (dir)
     {
      case Left:
         --Object ::pos.x;
         if (Object ::pos.x < 0)
           Object ::pos.x = 0;
         break;
      case Right:
         ++Object ::pos.x;
         //SCREEN: SCREEN SIZE?
         if (Object ::pos.x > 10000)
           Object ::pos.x = 10000;
         break;
      case Up:
         --Object ::pos.y;
         if (Object ::pos.y < 0)
           Object ::pos.y = 0;
         break;
      case Down:
         ++Object ::pos.y;
         //FIX: SCREEN SIZE?
         if (Object ::pos.y > 10000)
           Object ::pos.y = 10000;
         break;
     }
*/
//   if (cur_dir != dir)
//     Character ::cur_dir = dir;
   Position() += VECTOR2(dir * velocity);

   if (Position().x < 0) Position().x = 0;
   if (Position().y < 0) Position().y = 0;
   if (Position().x > SCRN_WIDTH) Position().x = SCRN_WIDTH;
   if (Position().y > SCRN_HEIGHT) Position().y = SCRN_HEIGHT;

   evas_object_move(this->img,
                    static_cast<Evas_Coord>(Position().x),
                    static_cast<Evas_Coord>(Position().y));

   return EINA_TRUE;
}

