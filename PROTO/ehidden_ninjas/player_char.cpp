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

#include "defines.h"
#include "player_char.h"

using namespace std;
using namespace ehninjas;

PlayerChar ::PlayerChar() :Character(string(""), ID_PLAYER)
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



Eina_Bool PlayerChar ::SetImgObj(Evas_Object* img_obj)
{
   return EINA_TRUE;
}



const Evas_Object *PlayerChar ::GetImgObj()
{
   return NULL;
}

