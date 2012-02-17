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

#include "defines.h"

namespace eHiddenNinja
{
   class Object
     {
      private:
#if DEBUG_MODE
        static unsigned int TOTAL_CNT;
#endif
        std:: string name;
        unsigned int id;

      public:
         Object(std:: string& name, std:: unsigned int& id) : name(name),
                                                              id(id)
           {
#if DEBUG_MODE
              ++TOTAL_CNT;
#endif
           }
         virtual ~Object()
           {
#if DEBUG_MODE
              --TOTAL_CNT;
#endif
           }

         std:: string& GetString()
           {
              return &this->name;
           }

         unsigned int GetId()
           {
              return this->id;
           }

         virtual Eina_Bool Initialize()
           {
              return EINA_TRUE;
           }

         virtual Eina_Bool Release()
           {
              return EINA_TRUE;
           }

         virtual SetImgObj(Evas_Object*) = 0;
         virtual const Evas_Object *GetImgObj() = 0;

         static void PrintDbgInfo()
           {
#if DEBUG_MODE
              PRINT_DBG("OBJECT COUNT: %d\n", Object ::TOTAL_CNT);
#endif
           }
     };

   class Block: public Object
     {
      public:
         Block() :Object(string(""), ID_BLOCK) {}
         ~Block() {}

      private:
        vector2<ELEMENT_TYPE> pos;
     }

}
