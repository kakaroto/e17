/*
 * Copyright 2012 Mike Blumenkrantz <michael.blumenkrantz@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "esql_module.h"

static void esql_module_desc_free(const Eina_Value_Struct_Operations *ops, const Eina_Value_Struct_Desc *desc, void *memory);
static void *esql_module_desc_alloc(const Eina_Value_Struct_Operations *ops, const Eina_Value_Struct_Desc *desc);
static const Eina_Value_Struct_Member *esql_module_desc_find_member(const Eina_Value_Struct_Operations *ops, const Eina_Value_Struct_Desc *desc, const char *name);

typedef struct Esql_Struct_Desc
{
   Eina_Value_Struct_Desc desc;
   Esql_Res *res;
} Esql_Struct_Desc;

static Eina_Value_Struct_Operations esql_module_desc_ops = {
  EINA_VALUE_STRUCT_OPERATIONS_VERSION,
  esql_module_desc_alloc,
  esql_module_desc_free,
  NULL, /* no copy */
  NULL, /* no compare */
  esql_module_desc_find_member
};

static void *
esql_module_desc_alloc(const Eina_Value_Struct_Operations *ops __UNUSED__, const Eina_Value_Struct_Desc *desc)
{
   Esql_Res *res;

   res = *(Esql_Res**)(((char*)desc) + desc->member_count * sizeof(Eina_Value_Struct_Member) + offsetof(Esql_Struct_Desc, res));
   if (res->mempool)
     return eina_mempool_malloc(res->mempool, desc->size);
   return malloc(desc->size);
}

static void
esql_module_desc_free(const Eina_Value_Struct_Operations *ops __UNUSED__, const Eina_Value_Struct_Desc *desc, void *memory)
{
   Esql_Res *res;

   res = *(Esql_Res**)(((char*)desc) + desc->member_count * sizeof(Eina_Value_Struct_Member) + offsetof(Esql_Struct_Desc, res));
   if (res->mempool)
     return eina_mempool_free(res->mempool, memory);
   free(memory);
}

static const Eina_Value_Struct_Member *
esql_module_desc_find_member(const Eina_Value_Struct_Operations *ops __UNUSED__, const Eina_Value_Struct_Desc *desc, const char *name)
{
   const Eina_Value_Struct_Member *itr, *itr_end;

   itr = desc->members;
   itr_end = itr + desc->member_count;

   /* assumes name is stringshared.
    *
    * we do this because it's the recommended usage pattern, moreover
    * we expect to find the member, as users shouldn't look for
    * non-existent members!
    */
   for (; itr < itr_end; itr++)
     if (itr->name == name)
       return itr;

   itr = desc->members;
   name = eina_stringshare_add(name);
   eina_stringshare_del(name); /* we'll not use the contents, this is fine */
   for (; itr < itr_end; itr++)
     if (itr->name == name)
       return itr;

   return NULL;
}

Eina_Value_Struct_Desc *
esql_module_desc_get(int cols, Esql_Module_Setup_Cb setup_cb, Esql_Res *res)
{
   Eina_Value_Struct_Desc *desc;
   int i;
   unsigned int offset;

   if (cols < 1) return NULL;
   if ((!setup_cb) || (!res)) return NULL;

   desc = malloc(sizeof(Esql_Struct_Desc) + cols * sizeof(Eina_Value_Struct_Member));
   EINA_SAFETY_ON_NULL_RETURN_VAL(desc, NULL);

   desc->version = EINA_VALUE_STRUCT_DESC_VERSION;
   desc->ops = &esql_module_desc_ops;
   desc->members = (void *)((char *)desc + sizeof(*desc));
   desc->member_count = cols;
   desc->size = 0;
   *(Esql_Res**)(((char*)desc) + cols * sizeof(Eina_Value_Struct_Member) + offsetof(Esql_Struct_Desc, res)) = res;

   offset = 0;
   for (i = 0; i < cols; i++)
     {
        Eina_Value_Struct_Member *m = (Eina_Value_Struct_Member *)desc->members + i;
        unsigned int size;

        m->offset = offset;
        setup_cb(res->backend.res, i, m);

        size = m->type->value_size;
        if (size % sizeof(void *) != 0)
          size += size - (size % sizeof(void *));

        offset += size;
     }

   desc->size = offset;
   res->mempool = esql_mempool_new(desc->size);
   return desc;
}
