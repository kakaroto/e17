#include <Eina.h>
#include <Ecore.h>

#include "Etam.h"

#include <etam_private.h>

static Etam_Packets *
_etam_packets_new(Etam_Collection *coll, const char *name, Etam_Data_Type type)
{
   Etam_Packets *r;

   eina_rwlock_take_write(&coll->lock);
   r = eina_hash_find(coll->rows, name);
   if (r) goto end;

   r = calloc(1, sizeof (Etam_Packets));
   if (!r) return NULL;

   r->type = type;
   eina_rwlock_new(&r->lock);

   eina_hash_add(coll->rows, name, r);

   EINA_REFCOUNT_INIT(r);

 end:
   eina_rwlock_release(&coll->lock);

   return r;
}

Etam_Packets *
etam_packets_rlock(Etam_Collection *coll, const char *name, Etam_Data_Type type)
{
   Etam_Packets *r;

   if (!coll) return NULL;

   eina_rwlock_take_read(&coll->lock);
   r = eina_hash_find(coll->rows, name);
   eina_rwlock_release(&coll->lock);

   if (!r)
     {
        r = _etam_packets_new(coll, name, type);
        if (!r) return NULL;
     }

   eina_rwlock_take_read(&r->lock);

   return r;
}

Etam_Packets *
etam_packets_wlock(Etam_Collection *coll, const char *name, Etam_Data_Type type)
{
   Etam_Packets *r;

   if (!coll) return NULL;

   eina_rwlock_take_read(&coll->lock);
   r = eina_hash_find(coll->rows, name);
   eina_rwlock_release(&coll->lock);

   if (!r)
     {
        r = _etam_packets_new(coll, name, type);
        if (!r) return NULL;
     }
   eina_rwlock_take_write(&r->lock);

   return r;
}

void
etam_packets_release(Etam_Packets *packets)
{
   eina_rwlock_release(&packets->lock);
}

void
etam_packets_free(Etam_Packets *packets)
{
   unsigned int i;
   unsigned short j;

   if (EINA_REFCOUNT_GET(packets))
     return ;

   eina_rwlock_free(&packets->lock);

   switch (packets->type)
     {
      case ETAM_T_BOOLEAN:
         for (i = 0; i < packets->packets_count; ++i)
           free(packets->u.boolean[i].rle_value);
         free(packets->u.boolean);
         break;
      case ETAM_T_LONG_LONG:
         for (i = 0; i < packets->packets_count; ++i)
           free(packets->u.long_long[i]);
         free(packets->u.long_long);
         break;
      case ETAM_T_STRING:
         /* It is assumed that all string are coming from Eet dictionnary, none have been directly inserted */
         for (i = 0; i < packets->packets_count; ++i)
           free(packets->u.string);
         free(packets->u.string);
         break;
      case ETAM_G_STRINGS:
         for (i = 0; i < packets->packets_count; ++i)
	   if (packets->u.strings[i])
	     for (j = 0; j < 1024; ++j)
	       eina_list_free(packets->u.strings[i]->values[j]);
         free(packets->u.strings);
         break;

      case ETAM_T_NONE:
      case ETAM_T_CHAR:
      case ETAM_T_UCHAR:
      case ETAM_T_SHORT:
      case ETAM_T_USHORT:
      case ETAM_T_INT:
      case ETAM_T_UINT:
      case ETAM_T_ULONG_LONG:
      case ETAM_T_FLOAT:
      case ETAM_T_DOUBLE:
      case ETAM_T_F32P32:
      case ETAM_T_F16P16:
      case ETAM_T_F8P24:
      case ETAM_T_NULL:
      case ETAM_T_RECTANGLE:
      case ETAM_G_RECTANGLES:
      case ETAM_G_TAGGED_RECTANGLES:
      default:
         fprintf(stderr, "Destruction of packet type %i impossible\n", packets->type);
     }

   free(packets);
}

void
etam_packets_del(Etam_Collection *coll, const char *name)
{
   Etam_Packets *r;

   if (!coll) return ;

   eina_rwlock_take_write(&coll->lock);

   r = eina_hash_find(coll->rows, name);
   if (!r) goto end;

   eina_hash_del(coll->rows, name, r);

   eina_rwlock_take_write(&r->lock);

   EINA_REFCOUNT_UNREF(r)
   {
      eina_rwlock_release(&r->lock);
      etam_packets_free(r);
      r = NULL;
   }

   if (r) eina_rwlock_release(&r->lock);

 end:
   eina_rwlock_release(&coll->lock);
}

