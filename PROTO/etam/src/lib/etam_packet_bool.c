#include <Eina.h>

#include "Etam.h"

#include "etam_private.h"

Eina_Bool
etam_packet_boolean_equal(Etam_Packets *packets, unsigned int block,
			  Eina_Bool value,
                          Etam_RLE_Bool *in, Etam_RLE_Bool *out)
{
   int idx_internal;
   int idx_in;
   int current_in;
   int current_internal;
   int offset_internal;
   int tmp;
   Eina_Bool r = EINA_FALSE;

   if (packets->type != ETAM_T_BOOLEAN) return  EINA_FALSE;

   eina_rwlock_take_read(&packets->lock);

   if (!(block < packets->packets_count))
     {
        eina_rwlock_release(&packets->lock);

        if (!value)
          {
             etam_rle_bool_copy(out, in);
             return EINA_TRUE;
          }

        return EINA_FALSE;
     }

   /* FIXME: This should really be implemented like an etam_rle_bool_and(out, in, packets->u.boolean[block]). */
#if 0
   idx_internal = 0;
   offset_internal = 0;
   current_internal = 0;

   ETAM_RLE_BOOL_FOREACH(in, idx_in, current_in, tmp)
     {
        Eina_Bool v;
        int c;

        /* Let's check if we have some boolean to catch up */
        while (current_internal < current_in - 1)
          {
             v = (packets->u.boolean[block].rle_value[idx_internal] & 1);
             c = (packets->u.boolean[block].rle_value[idx_internal] >> 1);
             c -= offset_internal;

             /* All this part is false in 'in', so push 'value == false' */
             while (c && current_internal < current_in - 1)
               {
                  etam_rle_bool_push(out, EINA_FALSE);
                  c--;
                  offset_internal++;
                  current_internal++;
               }

             if (!c)
               {
                  idx_internal++;
                  offset_internal = 0;

                  if (!(idx_internal < packets->u.boolean[block].length))
                    {
                       /* we are at the end of the packet */
                       goto end;
                    }
               }
          }

        v = (packets->u.boolean[block].rle_value[idx_internal] & 1);
        c = (packets->u.boolean[block].rle_value[idx_internal] >> 1);
        c -= ++offset_internal;
        current_internal++;

        etam_rle_bool_push(out, v == value);
        r |= (v == value);

        if (!c)
          {
             idx_internal++;
             offset_internal = 0;

             if (!(idx_internal < packets->u.boolean[block].length))
               {
                  /* we are at the end of the packet */
                  goto end;
               }
          }
     }

 end:
#endif

   eina_rwlock_release(&packets->lock);
}

Eina_Bool
etam_packet_boolean_set(Etam_Packets *packets, int idx, Eina_Bool value)
{
   unsigned int block;
   int internal;
   int idx_internal = 0;
   Eina_Bool r = EINA_FALSE;

   if (packets->type != ETAM_T_BOOLEAN) return EINA_FALSE;

   eina_rwlock_take_write(&packets->lock);

   block = idx >> 10;      /* divide by 1024 */
   internal = idx & 0x3FF; /* modulo 1024 */

   if (packets->packets_count < block)
     {
        Etam_RLE_Bool *tmp;

        tmp = realloc(packets->u.boolean, (block + 1) * sizeof (Etam_RLE_Bool));
        if (!tmp) goto on_error;

        packets->u.boolean = tmp;

        while (packets->packets_count < block + 1)
          {
             etam_rle_bool_init(packets->u.boolean + packets->packets_count);
             etam_rle_bool_set(packets->u.boolean + packets->packets_count++, EINA_FALSE, 0, 1024);
          }
     }

   /* Nothing at all, so fast build this array and fill it */
   if (!packets->u.boolean[block].length)
     {
        /* FIXME: this could be mucho faster, just dividing by 128 and insert quickly each char, but I am lazy */
        for (; internal > 1; internal--)
          etam_rle_bool_push(packets->u.boolean + block, EINA_FALSE);
        etam_rle_bool_push(packets->u.boolean + block, value);
        goto on_error;
     }

   /* FIXME: Shouldn't that be part of etam_bool infrastructure ? */
   /* Not really generic, but I know what I am doing, fast jump to the right position ! */
   while (internal > 0)
     {
        int c;

        c = (packets->u.boolean[block].rle_value[idx_internal] >> 1);
        if (internal - c <= 0)
          {
             unsigned char v;

             v = (packets->u.boolean[block].rle_value[idx_internal] & 1);
             if (v == value) goto on_error; /* This one is not an error, but we have nothing to do in that case */

             /* SPLIT IF NECESSARY */
             if (c == internal)
               {
                  packets->u.boolean[block].rle_value[idx_internal] -= 2;
                  if (packets->u.boolean[block].rle_value[idx_internal] < 2)
                    {
                       /* Nothing left in it ! */
                       packets->u.boolean[block].rle_value[idx_internal] = value ? 3 : 2;
                       goto on_error; /* nothing left to do */
                    }

                  packets->u.boolean[block].length++;
                  /* No space left ? */
                  if (packets->u.boolean[block].length > packets->u.boolean[block].memsize)
                    {
                       unsigned char *tmp;

                       /* Add some space */
                       packets->u.boolean[block].memsize += 32;
                       tmp = realloc(packets->u.boolean[block].rle_value, packets->u.boolean[block].memsize);
                       if (!tmp)
                         {
                            packets->u.boolean[block].memsize -= 32;
                            goto on_error;
                         }
                       packets->u.boolean[block].rle_value = tmp;
                    }

                  memmove(packets->u.boolean[block].rle_value + idx_internal + 1,
                          packets->u.boolean[block].rle_value + idx_internal,
                          packets->u.boolean[block].length - idx_internal - 2);
		  packets->u.boolean[block].rle_value[idx_internal] = value ? 3 : 2;
               }

	     internal -= c;
          }
        else
          {
             internal -= c;
             idx_internal++;

             /* checking if we have data left to process */
             if (idx_internal >= packets->u.boolean[block].length)
               {
                  /* FIXME: this could be mucho faster, just dividing by 128 and insert quickly each char, but I am lazy */
                  for (; internal > 1; internal--)
                    etam_rle_bool_push(packets->u.boolean + block, EINA_FALSE);
                  etam_rle_bool_push(packets->u.boolean + block, value);
                  internal = 0;
               }
          }
     }

   r = EINA_TRUE;

 on_error:
   eina_rwlock_release(&packets->lock);
   return r;
}

void
etam_packet_boolean_value_get(Etam_Packets *packets, int idx, Eina_Value *v)
{
}

void
etam_packet_boolean_data_get(Etam_Packets *packets, int idx, void *b)
{
}

void
etam_packets_boolean_value_get(Etam_Packets *packets, unsigned int block, Etam_RLE_Bool *map, Eina_Value *v)
{
}

void
etam_packets_boolean_data_get(Etam_Packets *packets, unsigned int block, Etam_RLE_Bool *map, void *a, int sizeof_struct, int offset)
{
}

