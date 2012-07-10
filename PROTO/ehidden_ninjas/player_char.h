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

#include <string>
#include <Evas.h>

#include "defines.h"
#include "object.h"

namespace ehninjas
{
   class Character;

   class PlayerChar : public Character
     {
        Evas_Object *img;

      public:
         PlayerChar();
         ~PlayerChar();
         Eina_Bool Initialize();
         Eina_Bool Release();
         Eina_Bool SetImgObj(Evas_Object*);
         const Evas_Object *GetImgObj();
         Eina_Bool Move(const VECTOR2&, ELEMENT_TYPE);
     };
}
