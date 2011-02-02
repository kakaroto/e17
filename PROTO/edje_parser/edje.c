/*
 * Copyright 2011 Mike Blumenkrantz <mike@zentific.com>
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <string.h>
#include <stdio.h>

#include "edje.h"
#ifdef HAVE_ALLOCA_H
#include <alloca.h>
#elif defined __GNUC__
#define alloca __builtin_alloca
#elif defined _AIX
#define alloca __alloca
#else
#include <stddef.h>
void *alloca(size_t);
#endif

const char *
edje_stringshare_toupper(const char *str)
{
   char *tmp;

   tmp = strdupa(str);
   eina_str_toupper(&tmp);
   return eina_stringshare_add(tmp);
}

Eina_Inlist *
edje_inlist_join(Eina_Inlist *a,
                 Eina_Inlist *b)
{
   Eina_Inlist *l;
   if (!a) return b;
   if (!b) return a;

   if ((!a->last) && (!b->last)) return eina_inlist_append(a, b);
   if ((!a->last) && b->last) return eina_inlist_prepend(b, a);
   if ((!b->last) && a->last) return eina_inlist_append(a, b);

   for (l = a->last, a->last = l->prev; l; l = a->last, a->last = l->prev)
     b = eina_inlist_prepend(b, l);

   return b;
}

#define DEF(TYPE, name)              \
  TYPE *                             \
  name##_new(void)                   \
  {                                  \
     return calloc(1, sizeof(TYPE)); \
  }

DEF(Edje, edje)
DEF(Edje_Collection, edje_collection)
DEF(Edje_Color_Class, edje_color_class)
DEF(Edje_External, edje_external)
DEF(Edje_Images, edje_images)
DEF(Edje_Image, edje_image)
DEF(Edje_Set_Image, edje_set_image)
DEF(Edje_Set, edje_set)
DEF(Edje_Font, edje_font)
DEF(Edje_Fonts, edje_fonts)
DEF(Edje_Group, edje_group)
DEF(Edje_Part, edje_part)
DEF(Edje_Program, edje_program)
DEF(Edje_Programs, edje_programs)

void
edje_param_free(Edje_Param *e)
{
   if (!e) return;

   switch (e->type)
     {
      case EDJE_PARAM_TYPE_STRING:
      case EDJE_PARAM_TYPE_CHOICE:
        eina_stringshare_del(e->data.s);
        break;

      default:
        break;
     }
   free(e);
}

Edje_Param *
edje_param_new(Edje_Param_Type type)
{
   Edje_Param *e;

   e = calloc(1, sizeof(Edje_Param));
   EINA_SAFETY_ON_NULL_RETURN_VAL(e, NULL);
   e->type = type;
   return e;
}

Edje_Data *
edje_data_new(void)
{
   Edje_Data *e;

   e = calloc(1, sizeof(Edje_Data));
   EINA_SAFETY_ON_NULL_RETURN_VAL(e, NULL);
   e->items = eina_hash_string_djb2_new((Eina_Free_Cb)eina_stringshare_del);
   e->files = eina_hash_string_djb2_new((Eina_Free_Cb)eina_stringshare_del);
   return e;
}

Edje_Style *
edje_style_new(void)
{
   Edje_Style *e;

   e = calloc(1, sizeof(Edje_Style));
   EINA_SAFETY_ON_NULL_RETURN_VAL(e, NULL);
   e->tags = eina_hash_string_djb2_new((Eina_Free_Cb)eina_stringshare_del);
   return e;
}

Edje_Part_Description *
edje_part_description_new(void)
{
   Edje_Part_Description *e;

   e = calloc(1, sizeof(Edje_Part_Description));
   EINA_SAFETY_ON_NULL_RETURN_VAL(e, NULL);
   e->params = eina_hash_string_djb2_new((Eina_Free_Cb)edje_param_free);
   return e;
}

Edje_Parts *
edje_parts_new(void)
{
   Edje_Parts *e;

   e = calloc(1, sizeof(Edje_Parts));
   EINA_SAFETY_ON_NULL_RETURN_VAL(e, NULL);
   e->aliases = eina_hash_string_djb2_new((Eina_Free_Cb)eina_stringshare_del);
   return e;
}

