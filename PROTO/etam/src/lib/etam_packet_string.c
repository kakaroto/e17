#include <fnmatch.h>

#include <Eina.h>

#include "Etam.h"

#include "etam_private.h"

/* It is assumed that value is a pointer to a string from eet dictionnary. */
Eina_Bool
etam_packet_string_equal(Etam_Packets *packets, unsigned int block,
                         const char *value,
                         Etam_RLE_Bool *in, Etam_RLE_Bool *out)
{
   int idx_in;
   int current_in;
   int current_out;
   int tmp;
   Eina_Bool r = EINA_FALSE;

   if (packets->type != ETAM_T_STRING) return EINA_FALSE;

   eina_rwlock_take_read(&packets->lock);

   if (!(block < packets->packets_count) || !packets->u.string[block])
     {
        eina_rwlock_release(&packets->lock);

        return EINA_FALSE;
     }

   current_out = 0;

   ETAM_RLE_BOOL_FOREACH(in, idx_in, current_in, tmp)
     {
        Eina_Bool v;

        /* This means that all previous entry where wrong, so catching up */
        /* FIXME: use etam_rle_bool_set */
        while (current_out < current_in - 1)
          {
             etam_rle_bool_push(out, EINA_FALSE);
             current_out++;
          }

        /* we can do that because string in eet are always shared. */
        v = packets->u.string[block]->value[current_out] == value;
        etam_rle_bool_push(out, v);
        r |= v;
     }

   eina_rwlock_release(&packets->lock);

   return r;
}

Eina_Bool
etam_packet_string_like(Etam_Packets *packets, unsigned int block,
                        const char *value, int flags /* this should be the fnmatch flags */,
                        Etam_RLE_Bool *in, Etam_RLE_Bool *out)
{
   int idx_in;
   int current_in;
   int current_out;
   int tmp;
   Eina_Bool r = EINA_FALSE;

   if (packets->type != ETAM_T_STRING) return EINA_FALSE;

   eina_rwlock_take_read(&packets->lock);

   if (!(block < packets->packets_count) || !packets->u.string[block])
     {
        eina_rwlock_release(&packets->lock);

        return EINA_FALSE;
     }

   current_out = 0;

   ETAM_RLE_BOOL_FOREACH(in, idx_in, current_in, tmp)
     {
        Eina_Bool v;

        /* This means that all previous entry where wrong, so catching up */
        /* FIXME: use etam_rle_bool_set */
        while (current_out < current_in - 1)
          {
             etam_rle_bool_push(out, EINA_FALSE);
             current_out++;
          }

        /* we can do that because string in eet are always shared. */
        v = fnmatch(value, packets->u.string[block]->value[current_out], flags) == 0;
        etam_rle_bool_push(out, v);
        r |= v;
     }

   eina_rwlock_release(&packets->lock);

   return r;
}

/* It is assumed that s is already a shared string in eet dictionary */
Eina_Bool
etam_packet_string_set(Etam_Packets *packets, int idx, const char *s)
{
   unsigned int block;
   unsigned int internal;

   if (packets->type != ETAM_T_STRING) return EINA_FALSE;

   block = idx >> 10; /* divide by 1024 */
   internal = idx & 0x3FF; /* modulo 1024 */

   eina_rwlock_take_write(&packets->lock);

   if (!(block < packets->packets_count))
     {
        Etam_Packet_String **tmp;

        tmp = realloc(packets->u.string, (block + 1) * sizeof (Etam_Packet_String *));
        if (!tmp) goto on_error;
        packets->u.string = tmp;
        memset(packets->u.string + packets->packets_count, 0, block + 1 - packets->packets_count);
        packets->packets_count = block + 1;
     }

   if (!packets->u.string[block])
     {
        packets->u.string[block] = calloc(1, sizeof (Etam_Packet_String));
        if (!packets->u.string[block]) goto on_error;
     }

   packets->u.string[block]->value[internal] = s;

   eina_rwlock_release(&packets->lock);
   return EINA_TRUE;

 on_error:
   eina_rwlock_release(&packets->lock);
   return EINA_FALSE;
}

void
etam_packet_string_value_get(Etam_Packets *packets, int idx, Eina_Value *v)
{
   unsigned int block;
   unsigned int internal;

   /* Eina_Value should be able to store static string that are available for the complete life of a program */
   eina_value_setup(v, EINA_VALUE_TYPE_STRING);

   if (packets->type != ETAM_T_STRING)
     goto on_error;

   block = idx >> 10; /* divide by 1024 */
   internal = idx & 0x3FF; /* modulo 1024 */

   eina_rwlock_take_read(&packets->lock);

   if (!(block < packets->packets_count) || !packets->u.string[block])
     {
        eina_rwlock_release(&packets->lock);
        goto on_error;
     }

   eina_value_set(v, packets->u.string[block]->value[internal]);

   eina_rwlock_release(&packets->lock);

   return ;

 on_error:
   eina_value_set(v, NULL);
}

void
etam_packet_string_data_get(Etam_Packets *packets, int idx, void *v)
{
   const char **value = v;
   unsigned int block;
   unsigned int internal;

   if (packets->type != ETAM_T_STRING)
     goto on_error;

   block = idx >> 10; /* divide by 1024 */
   internal = idx & 0x3FF; /* modulo 1024 */

   eina_rwlock_take_read(&packets->lock);

   if (!(block < packets->packets_count) || !packets->u.string[block])
     goto on_error;

   *value = packets->u.string[block]->value[internal];

   eina_rwlock_release(&packets->lock);

   return ;

on_error:
   *value = NULL;
}

void
etam_packets_string_value_get(Etam_Packets *packets, unsigned int block, Etam_RLE_Bool *map, Eina_Value *v)
{
   int idx_in;
   int current_in;
   int tmp;
   int i = 0;

   if (packets->type != ETAM_T_STRING)
     return ;

   eina_rwlock_take_read(&packets->lock);

   if (!(block < packets->packets_count) || !packets->u.string[block])
     goto on_error;

   ETAM_RLE_BOOL_FOREACH(map, idx_in, current_in, tmp)
     {
        Eina_Value *st;

        eina_value_array_get(v, i, &st);

        eina_value_struct_set(st, packets->name, packets->u.string[block]->value[current_in]);
        i++;
     }

 on_error:
   eina_rwlock_release(&packets->lock);
}

void
etam_packets_string_data_get(Etam_Packets *packets, unsigned int block, Etam_RLE_Bool *map,
                             void *a, int sizeof_struct, int offset)
{
   unsigned char *addr;
   int idx_in;
   int current_in;
   int tmp;
   int i = 0;

   if (packets->type != ETAM_T_STRING)
     return ;

   eina_rwlock_take_read(&packets->lock);

   if (!(block < packets->packets_count) || !packets->u.string[block])
     goto on_error;

   ETAM_RLE_BOOL_FOREACH(map, idx_in, current_in, tmp)
     {
        const char **value;

        addr = ((unsigned char *) a) + sizeof_struct * i + offset;
        value = (const char **) addr;

        *value = packets->u.string[block]->value[current_in];
        i++;
     }

 on_error:
   eina_rwlock_release(&packets->lock);
}


