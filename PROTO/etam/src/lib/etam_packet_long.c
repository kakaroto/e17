#include <Eina.h>

#include "Etam.h"

#include "etam_private.h"

/**
 * Various notes:
 * --------------
 * - Out is supposed to be already initialised.
 * - It will return EINA_FALSE if there is no true value pushed in.
 * - If there is no data in the block, 0 is assumed to be the default value
 */

#define ETAM_PACKET_LONG_LONG_CMP(Packets, Block, Value, In, Out, Cmp)  \
  {                                                                     \
     int idx_in;                                                        \
     int current_in;                                                    \
     int current_out;                                                   \
     int tmp;                                                           \
     Eina_Bool r = EINA_FALSE;                                          \
                                                                        \
     if (Packets->type != ETAM_T_LONG_LONG) return EINA_FALSE;          \
                                                                        \
     eina_rwlock_take_read(&Packets->lock);                             \
                                                                        \
     if (!(Block < Packets->packets_count) || !Packets->u.long_long[Block]) \
       {                                                                \
          eina_rwlock_release(&Packets->lock);                          \
                                                                        \
          if (Value == 0)                                               \
            {                                                           \
               etam_rle_bool_copy(Out, In);                             \
               return EINA_TRUE;                                        \
            }                                                           \
          return EINA_FALSE;                                            \
       }                                                                \
                                                                        \
     current_out = 0;                                                   \
                                                                        \
     ETAM_RLE_BOOL_FOREACH(In, idx_in, current_in, tmp)                 \
       {                                                                \
          Eina_Bool v;                                                  \
                                                                        \
          /* This means that all previous entry where wrong, so catching up */ \
          /* FIXME: use etam_rle_bool_set */                            \
          while (current_out < current_in - 1)                          \
            {                                                           \
               etam_rle_bool_push(Out, EINA_FALSE);                     \
               current_out++;                                           \
            }                                                           \
                                                                        \
          v = (Packets->u.long_long[Block]->value[current_out] Cmp Value); \
          etam_rle_bool_push(Out, v);                                   \
          r |= v;                                                       \
       }                                                                \
                                                                        \
     eina_rwlock_release(&Packets->lock);                               \
                                                                        \
     return r;                                                          \
  }

Eina_Bool
etam_packet_long_long_equal(Etam_Packets *packets, unsigned int block,
			    long long value,
			    Etam_RLE_Bool *in, Etam_RLE_Bool *out)
{
   ETAM_PACKET_LONG_LONG_CMP(packets, block, value, in, out, ==);
}

Eina_Bool
etam_packet_long_long_inf(Etam_Packets *packets, unsigned int block,
			  long long value,
			  Etam_RLE_Bool *in, Etam_RLE_Bool *out)
{
   ETAM_PACKET_LONG_LONG_CMP(packets, block, value, in, out, <);
}

Eina_Bool
etam_packet_long_long_sup(Etam_Packets *packets, unsigned int block,
                          long long value,
                          Etam_RLE_Bool *in, Etam_RLE_Bool *out)
{
   ETAM_PACKET_LONG_LONG_CMP(packets, block, value, in, out, >);
}

Eina_Bool
etam_packet_long_long_set(Etam_Packets *packets, int idx, long long ll)
{
   unsigned int block;
   unsigned int internal;

   if (packets->type != ETAM_T_LONG_LONG) return EINA_FALSE;

   block = idx >> 10; /* divide by 1024 */
   internal = idx & 0x3FF; /* modulo 1024 */

   /* Should find a light way to have a more fine grained write lock */
   eina_rwlock_take_write(&packets->lock);

   if (!(block < packets->packets_count))
     {
        Etam_Packet_Long_Long **tmp;

        tmp = realloc(packets->u.long_long, (block + 1) * sizeof (Etam_Packet_Long_Long *));
        if (!tmp) goto on_error;
        packets->u.long_long = tmp;
        memset(packets->u.long_long + packets->packets_count, 0, block + 1 - packets->packets_count);
        packets->packets_count = block + 1;
     }

   if (!packets->u.long_long[block])
     {
        packets->u.long_long[block] = calloc(1, sizeof (Etam_Packet_Long_Long));
        if (!packets->u.long_long[block]) goto on_error;
     }

   packets->u.long_long[block]->value[internal] = ll;

   eina_rwlock_release(&packets->lock);
   return EINA_TRUE;

 on_error:
   eina_rwlock_release(&packets->lock);
   return EINA_FALSE;
}

void
etam_packet_long_long_value_get(Etam_Packets *packets, int idx, Eina_Value *v)
{
   unsigned int block;
   unsigned int internal;

   eina_value_setup(v, EINA_VALUE_TYPE_INT64);

   if (packets->type != ETAM_T_LONG_LONG)
     goto on_error;

   block = idx >> 10; /* divide by 1024 */
   internal = idx & 0x3FF; /* modulo 1024 */

   eina_rwlock_take_read(&packets->lock);

   if (!(block < packets->packets_count) || !packets->u.long_long[block])
     {
        eina_rwlock_release(&packets->lock);
        goto on_error;
     }

   eina_value_set(v, packets->u.long_long[block]->value[internal]);

   eina_rwlock_release(&packets->lock);

   return ;

 on_error:
   eina_value_set(v, 0);   
}

void
etam_packet_long_long_data_get(Etam_Packets *packets, int idx, void *v)
{
   long long *value = v;
   unsigned int block;
   unsigned int internal;

   if (packets->type != ETAM_T_LONG_LONG)
     goto on_error;

   block = idx >> 10; /* divide by 1024 */
   internal = idx & 0x3FF; /* modulo 1024 */

   eina_rwlock_take_read(&packets->lock);

   if (!(block < packets->packets_count) || !packets->u.long_long[block])
     goto on_error;

   *value = packets->u.long_long[block]->value[internal];

   eina_rwlock_release(&packets->lock);

   return ;

on_error:
   *value = 0;
}

/* It is assumed that Eina_Value is a an initialized array of EINA_VALUE_TYPE_STRUCT and this function
 * will put a new entry in each struct.   
 */
void
etam_packets_long_long_value_get(Etam_Packets *packets, unsigned int block, Etam_RLE_Bool *map, Eina_Value *v)
{
   int idx_in;
   int current_in;
   int tmp;
   int i = 0;

   if (packets->type != ETAM_T_LONG_LONG)
     return ;

   eina_rwlock_take_read(&packets->lock);

   if (!(block < packets->packets_count) || !packets->u.long_long[block])
     goto on_error;

   ETAM_RLE_BOOL_FOREACH(map, idx_in, current_in, tmp)
     {
        Eina_Value *st;

        eina_value_array_get(v, i, &st);

        eina_value_struct_set(st, packets->name, packets->u.long_long[block]->value[current_in]);
        i++;
     }

 on_error:
   eina_rwlock_release(&packets->lock);
}

void
etam_packets_long_long_data_get(Etam_Packets *packets, unsigned int block, Etam_RLE_Bool *map, void *a,
                                int sizeof_struct, int offset)
{
   unsigned char *addr;
   int idx_in;
   int current_in;
   int tmp;
   int i = 0;

   if (packets->type != ETAM_T_LONG_LONG)
     return ;

   eina_rwlock_take_read(&packets->lock);

   if (!(block < packets->packets_count) || !packets->u.long_long[block])
     goto on_error;

   ETAM_RLE_BOOL_FOREACH(map, idx_in, current_in, tmp)
     {
        long long *value;

        addr = ((unsigned char *) a) + sizeof_struct * i + offset;
        value = (long long *) addr;

        *value = packets->u.long_long[block]->value[current_in];
        i++;
     }

 on_error:
   eina_rwlock_release(&packets->lock);
}

