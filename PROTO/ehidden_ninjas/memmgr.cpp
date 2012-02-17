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

#include <Eina.h>
#include "singleton.h"
#include "memmgr.h"

using namespace std;
using namespace ehninjas;


Eina_Bool MemoryMgr ::initialized = EINA_FALSE;

MemoryMgr ::MemoryMgr() :modules(NULL), mempool(NULL), pool_size(0)
{
   //load mempool modules
   this->modules = eina_module_list_get(NULL,
                                        MPDIR "/src/modules",
                                        EINA_TRUE,
                                        NULL,
                                        NULL);
   eina_module_list_load(modules);
}

MemoryMgr ::~MemoryMgr()
{
   Terminate();
   eina_module_list_free(modules);
}

Eina_Bool MemoryMgr ::Initialize(unsigned int pool_size)
{
   assert(!this->initialized && (pool_size > 0));

   this->mempool = eina_mempool_add("chained_mempool",
                                    "ehninjas",
                                    NULL,
                                    sizeof(size_t),
                                    this->pool_size);
   if (!this->mempool)
     return EINA_FALSE;

   MemoryMgr ::initialized = EINA_TRUE;

   return EINA_TRUE;
}

void MemoryMgr ::Terminate()
{
   if (this->initialized == EINA_FALSE) return;
   eina_mempool_del(this->mempool);
   this->initialized = EINA_FALSE;
}

void *MemoryMgr ::Alloc(unsigned int size)
{
   assert(this->initialized && (size > 0));
   return eina_mempool_malloc(this->mempool, size);
}

void *MemoryMgr ::Realloc(void *p, unsigned int size)
{
   assert(this->initialized && p && (size > 0));
   return eina_mempool_realloc(this->mempool, p, size);
}

void MemoryMgr ::Free(void *p)
{
   assert(this->initialized && p);
   eina_mempool_free(this->mempool, p);
}

unsigned int MemoryMgr ::GetPoolSize()
{
   return this->pool_size;
}
