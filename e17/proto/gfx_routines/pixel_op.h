#ifndef _PIXEL_OP_H
#define _PIXEL_OP_H

#ifndef DATA32
typedef unsigned long long  DATA64;
typedef unsigned int        DATA32;
typedef unsigned short      DATA16;
typedef unsigned char       DATA8;
#endif

typedef struct _Pixel_Op_Params Pixel_Op_Params;
typedef void (*Pixel_Op_Func) (Pixel_Op_Params *params);

#define PIXEL_OP_VERSION 1

typedef enum _Pixel_Op
{
   PIXEL_OP_COPY,
   PIXEL_OP_BLEND,
   PIXEL_OP_ADD,
   PIXEL_OP_SUB,
   PIXEL_OP_LAST
} Pixel_Op;

struct _Pixel_Op_Params
{
   DATA32   v;  /* pixel op parameter version - 1 to start */
   DATA32   l;  /* run length in pixels - always > 0 */
   Pixel_Op op; /* one of the supported ops in Pixel_Op */
   struct {
      DATA32 *p; /* color and alpha pixel pointer - NULL       == not used */
      DATA8  *m; /* alpha mask pixel pointer      - NULL       == not used */
      DATA32  c; /* color multiplier              - 0xffffffff == not used */
      DATA8   alpha  : 1; /* pixels contain alpha */
      DATA8   sparse : 1; /* pixels have sparse alpha (mostly 0, or 255) */
   } src, dst;
   /* -- */
   /* expanded parameters go here in future - if we expand we need to go up */
   /* a version number so we know if the request was old or new style */
   /* -- */
};

int           pixel_op_init(void);
int           pixel_op_shutdown(void);
inline void   pixel_op_params_init(Pixel_Op_Params *params, DATA32 v);
#define       PO_INIT(pp) pixel_op_params_init(pp, PIXEL_OP_VERSION);
Pixel_Op_Func pixel_op_get(Pixel_Op_Params *params, int cpumode);
void          pixel_op(Pixel_Op_Params *params);
void          pixel_op_end(void);
    
#endif
