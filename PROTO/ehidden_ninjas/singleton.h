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

#include <cassert>

namespace ehninjas
{

   template<typename T>
   class Singleton
     {
      private:
         static T *instance;

      public:
         Singleton();
         virtual ~Singleton();
         inline static T *GetSingleton();
        };

   template<typename T> T *Singleton<T>:: instance = NULL;

   template<typename T>
   Singleton<T>:: Singleton()
     {
       assert(this->instance == NULL);

       //Address offset: Derived Class - Singleton Class
       T *base = reinterpret_cast<T*>(1);
       Singleton<T> *singleton = reinterpret_cast<Singleton<T>*>(base);
       size_t offset = reinterpret_cast<size_t>(base) -
                       reinterpret_cast<size_t>(singleton);

       size_t inst_addr = reinterpret_cast<size_t>(this) + offset;
       this->instance = reinterpret_cast<T*>(inst_addr);
      }

   template<typename T>
   Singleton<T>:: ~Singleton()
     {
        assert(this->instance != NULL);
        this->instance = NULL;
     }

   template<typename T>
   T* Singleton<T>:: GetSingleton()
     {
        assert(instance != NULL);
        return instance;
     }
}

