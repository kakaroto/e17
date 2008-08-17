#ifndef _PIXEL_OP_H
#define _PIXEL_OP_H

#ifndef DATA32
typedef unsigned long long  DATA64;
typedef unsigned int        DATA32;
typedef unsigned short      DATA16;
typedef unsigned char       DATA8;
#endif

/* src pixel flags: */

/* pixels none */
#define SP_N 0
/* pixels (argb default) */
#define SP 1
/* pixels are rgb (ie. alphas == 255) */
#define SP_AN 2
/* pixels alpha are sparse */
#define SP_AS 3
/* src pixels flags count */
#define SP_LAST 4

/* src mask flags: */

/* mask none */
#define SM_N 0
/* mask (alpha) */
#define SM 1
/* mask alphas are 'trivial - ie. only 0 or 255 */
#define SM_AT 2
/* mask alphas are sparse */
#define SM_AS 3
/* src mask flags count */
#define SM_LAST 4

/* src color flags: */

/* color is 0xffffffff */
#define SC_N 0
/* color (argb default) */
#define SC 1
/* color is rgb (ie. 0xffrrggbb) */
#define SC_AN 2
/* color is 'grey' (ie. 0xaaaaaaaa) */
#define SC_AA 3
/* src color flags count */
#define SC_LAST 4

/* dst pixels flags: */

/* pixels (argb default) */
#define DP  0
/* pixels are rgb (ie. alphas == 255) */
#define DP_AN  1
/* pixels alpha sparse */
#define DP_AS 2
/* dst pixels flags count */
#define DP_LAST 3

/* possible other uses... */
#define DM 3
#define DM_AS 4
#define DM_LAST 5

#define DC  5
#define DC_AN  6
#define DC_AA 7
#define DC_LAST 8

/* dst flags count */
#define D_LAST 8

typedef struct _Pixel_Op_Params Pixel_Op_Params;
typedef void (*Pixel_Op_Func) (Pixel_Op_Params *params);

#define PIXEL_OP_VERSION 1

typedef enum _Pixel_Op
{
   PIXEL_OP_COPY,
   PIXEL_OP_BLEND,
   PIXEL_OP_ADD,
   PIXEL_OP_SUB,
   PIXEL_OP_MUL,
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
      DATA8   pa; /* property describing the pixel alpha channel */
      DATA8   ma; /* property describing the mask alpha channel */
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
