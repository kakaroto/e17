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

namespace ehninjas
{
   class Object
     {
      private:
        std:: string name;
        unsigned int id;
        Evas_Coord_Point size;
        VECTOR2 pos;

      public:
        Object(std:: string name, unsigned int id) : name(name),
                                                     id(id),
                                                     pos(VECTOR2()) {}
         virtual ~Object() {}

         std:: string& GetString() { return this->name; }
         unsigned int GetId() { return this->id; }
         VECTOR2 &Position() { return this->pos; }
         Evas_Coord_Point &Size() { return this->size; }
         virtual Eina_Bool Initialize() { return EINA_TRUE; }
         virtual Eina_Bool Release() { return EINA_TRUE; }

         virtual Eina_Bool SetImgObj(Evas_Object*) = 0;
         virtual const Evas_Object *GetImgObj() = 0;
     };

   class Block: public Object
     {
      public:
         Block() :Object(std ::string(""), ID_BLOCK) {}
         ~Block() {}

         Eina_Bool SetImgObj(Evas_Object*) { return EINA_TRUE; }
         const Evas_Object *GetImgObj() { return NULL; }
     };

   class Character : public Object
     {
        enum eMoveDirection { Left, Right, Up, Down };

      public:
         Character(const std ::string name, unsigned int id) : Object(name, id)
         {}
         virtual Eina_Bool Move(unsigned int) { return EINA_TRUE; }
     };

   class NonPlayerChar : public Character
     {
         NonPlayerChar();
         ~NonPlayerChar();
         Eina_Bool Initialize() { return EINA_TRUE; }
         Eina_Bool Release() { return EINA_TRUE; }
         Eina_Bool SetImgObj(Evas_Object*);
         const Evas_Object *GetImgObj();
     };

}
