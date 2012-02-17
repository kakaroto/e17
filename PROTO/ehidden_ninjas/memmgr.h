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

namespace ehninjas
{
   class singleton;

   class MemoryMgr : public Singleton<MemoryMgr>
     {
      private:
         static Eina_Bool initialized;
         Eina_Array *modules;
         Eina_Mempool *mempool;
         unsigned int pool_size;

      public:
         MemoryMgr();
         ~MemoryMgr();

         Eina_Bool Initialize(unsigned int);
         void Terminate();
         void *Alloc(unsigned int size);
         void *Realloc(void *, unsigned int size);
         void Free(void *);
         unsigned int GetPoolSize();
     };
}

