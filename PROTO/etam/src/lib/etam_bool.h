#ifndef ETAM_BOOL_H_
# define ETAM_BOOL_H_

#include <Eina.h>

typedef struct _Etam_RLE_Bool Etam_RLE_Bool;

struct _Etam_RLE_Bool {
   int length;
   int memsize;
   unsigned char *rle_value;
};

static inline void etam_rle_bool_init(Etam_RLE_Bool *rle);
static inline void etam_rle_bool_reset(Etam_RLE_Bool *rle);
static inline void etam_rle_bool_copy(Etam_RLE_Bool *out, Etam_RLE_Bool *in);
static inline Eina_Bool etam_rle_bool_push(Etam_RLE_Bool *rle, Eina_Bool add);
static inline void etam_rle_bool_set(Etam_RLE_Bool *rle, Eina_Bool v,
				     int start, int length);

#define ETAM_RLE_BOOL_FOREACH(Rle, Idx, Current, Tmp)                   \
  for (Idx = 0, Current = 0; Idx < Rle->length; Idx++)                  \
    if (!(Rle->rle_value[Idx] & 1))					\
      {                                                                 \
	 Current += Rle->rle_value[Idx] >> 1;                           \
      }                                                                 \
    else                                                                \
      for (Tmp = Rle->rle_value[Idx] >> 1; Tmp > 0; Tmp--, Current++)


static inline void
etam_rle_bool_init(Etam_RLE_Bool *rle)
{
   rle->length = 0;
   rle->memsize = 0;
   rle->rle_value = NULL;
}

static inline void
etam_rle_bool_reset(Etam_RLE_Bool *rle)
{
   rle->length = 0;
}

static inline void
etam_rle_bool_copy(Etam_RLE_Bool *out, Etam_RLE_Bool *in)
{
   out->length = in->length;
   out->memsize = in->length;
   out->rle_value = realloc(out->rle_value, out->length * sizeof (unsigned char));
   if (!out->rle_value)
     out->length = out->memsize = 0;
   else
     memcpy(out->rle_value, in->rle_value, out->length);
}

static inline Eina_Bool
etam_rle_bool_growth(Etam_RLE_Bool *rle, unsigned int limit)
{
   unsigned char *tmp;

   if (limit < rle->memsize) return EINA_TRUE;

   rle->memsize += 8;

   tmp = realloc(rle->rle_value, sizeof (unsigned char) * rle->memsize);
   if (!tmp)
     {
        rle->memsize -= 8;
        return EINA_FALSE;
     }

   rle->rle_value = tmp;
   return EINA_TRUE;
}

static inline void
etam_rle_bool_set(Etam_RLE_Bool *rle, Eina_Bool v, int start, int length)
{
   int idx_internal = 0;
   int i;

   v = v & 1;

   for (i = 0; i < start && idx_internal < rle->length; idx_internal++)
     {
        i += (rle->rle_value[idx_internal] >> 1);
     }

   /* adjust start boundary */
   if (i > start)
     {
        if (((rle->rle_value[idx_internal - 1] & 1) & v) == v)
          {
             /* actually the stored value is already good */
             length -= i - start;
             start = i;
          }
        else
          {
             /* So we are removing the start from the current compressed index */
             int tmp;

             tmp = rle->rle_value[idx_internal - 1] >> 1;
             tmp = tmp - (i - start);
             rle->rle_value[idx_internal - 1] = tmp << 1;
             if (!v) rle->rle_value[idx_internal - 1] |= 1;
          }
     }

   /* move things around */
   if (idx_internal < rle->length)
     {
        /* FIXME: Handle nasty insertion here */
     }

   /* pad the end of the rle */
   if (length > 0)
     {
        if (rle->memsize - idx_internal < length)
          {
             unsigned char *tmp;
             int memsize;

             memsize = rle->memsize - idx_internal + length;
             memsize |= 7;

             tmp = realloc(rle->rle_value, memsize);
             if (!tmp) return ; /* that's a very bad situation */

             rle->rle_value = tmp;
             rle->memsize = memsize;
          }

        while (length > 0)
          {
             rle->rle_value[idx_internal++] = 0xFE | v;
             rle->length++;
             length -= 0xFE;
          }

        if (length < 0)
          {
             rle->rle_value[idx_internal - 1] = (0xFE + length) | v;
          }
     }
}

static inline Eina_Bool
etam_rle_bool_push(Etam_RLE_Bool *rle, Eina_Bool add)
{
   if (rle->length == 0                                      /* - No previous value */
       || !((rle->rle_value[rle->length - 1] & 1) & !!add)    /* - Not the same value as before */
       || ((rle->rle_value[rle->length - 1] & 0xFE) == 0xFE)) /* - Previous value is full */
     {
        rle->length++;

        if (!etam_rle_bool_growth(rle, rle->length))
          return EINA_FALSE;

        rle->rle_value[rle->length - 1] = add ? 3 : 2;
     }
   else
     {
        rle->rle_value[rle->length - 1] += 2;
     }

   return EINA_TRUE;
}

static inline Eina_Bool
etam_rle_bool_and(Etam_RLE_Bool *out, const Etam_RLE_Bool *a, const Etam_RLE_Bool *b)
{
   Eina_Bool ba, bb;
   int ca, cb;
   int oa, ob;
   int cr, or;
   int c;

   oa = 0; ob = 0;
   ca = 0;
   cb = (b->rle_value[ob]) >> 1; bb = b->rle_value[ob] & 1;

   while (oa < a->length && ob < b->length)
     {
        if (ca == 0)
          {
             ca = (a->rle_value[oa]) >> 1;
             ba = a->rle_value[oa] & 1;
          }
        if (cb == 0)
          {
             cb = (b->rle_value[ob]) >> 1;
             bb = b->rle_value[ob] & 1;
          }

        c = (ca < cb) ? ca : cb;

        if (!etam_rle_bool_growth(out, or))
          return EINA_FALSE;

        out->rle_value[or++] = (c << 1) | (ba & bb);
        ca -= c;
        cb -= c;

        if (ca == 0) oa++;
        if (cb == 0) ob++;
     }

   if (ca || oa < a->length)
     {
        if (!etam_rle_bool_growth(out, or))
          return EINA_FALSE;

        out->rle_value[or++] = (ca << 1) | ba;

        while (oa < a->length)
          {
             if (!etam_rle_bool_growth(out, or))
               return EINA_FALSE;

             out->rle_value[or++] = a->rle_value[oa++];
          }
     }
   else if (cb || ob < b->length)
     {
        if (!etam_rle_bool_growth(out, or))
          return EINA_FALSE;

        out->rle_value[or++] = (cb << 1) | bb;

        while (ob < b->length)
          {
             if (!etam_rle_bool_growth(out, or))
               return EINA_FALSE;

             out->rle_value[or++] = b->rle_value[ob++];
          }
     }

   out->length = or;

   return EINA_TRUE;
}

#endif
