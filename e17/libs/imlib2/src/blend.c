#include "common.h"
#include <X11/Xlib.h>
#include "colormod.h"
#include "image.h"
#include "blend.h"
#include "scale.h"

/*
 * Thanks to Ryan Gustafson <ryan_gustafson@yahoo.com> fro the reduction in
 * size of this file with lots of macros.. :)
 * 
 * These operations are over a rectangular region in each of the images. The
 * macros below provide common code for looping over the region.  The pointers
 * 'p1' and 'p2' point to the current pixel.
 *
 * There are several version of the LOOP_START macro so that -Wall compilations
 * will no complain about unused variables.  Use as appropriate.
 */

/* Just enough variables to loop, no color variables at all. */
#define LOOP_START                                   \
   int x, y;                                         \
   DATA32 *p1, *p2;                                  \
                                                     \
   p1 = src;                                         \
   p2 = dst;                                         \
   for (y = h; y > 0; y--)                           \
     {                                               \
	for (x = w; x > 0; x--)                      \
	  {

/* Add tmp calculation variable. */
#define LOOP_START_0                                 \
   int tmp;                                          \
   LOOP_START

/* Add variables to read pixel colors (no alpha) */
#define LOOP_START_1                                 \
   DATA8 r, g, b, rr, gg, bb, nr, ng, nb;            \
   LOOP_START_0

/* Add source alpha. */
#define LOOP_START_2                                 \
   DATA8 a;                                          \
   LOOP_START_1

/* Add destination and new alpha. */
#define LOOP_START_3                                 \
   DATA8 aa, na;                                     \
   LOOP_START_2
		
#define LOOP_END                                     \
	  }                                          \
	p1 += src_jump;                              \
	p2 += dst_jump;                              \
     }

#define LOOP_END_WITH_INCREMENT                      \
   p1++;                                             \
   p2++;                                             \
   LOOP_END

/*
 * These macros are handy for reading/writing pixel color information from/to
 * packed integers.  The packed ordering of both the input and output images
 * is assumed to be ARGB.
 */

/************************************************************************
 * **********************************************************************
#define READ_RGB(p, r, g, b)  \
   (r) = (*(p) >> 16) & 0xff; \
   (g) = (*(p) >> 8 ) & 0xff; \
   (b) = (*(p)      ) & 0xff;

#define READ_ALPHA(p, a) \
   (a) = ((*(p)) >> 24) & 0xff;

#define READ_RGBA(p, r, g, b, a) \
   (a) = (*(p) >> 24) & 0xff;    \
   (r) = (*(p) >> 16) & 0xff;    \
   (g) = (*(p) >> 8 ) & 0xff;    \
   (b) = (*(p)      ) & 0xff;

#define WRITE_RGB(p, r, g, b) \
   *(p) = ((r) << 16) | ((g) << 8) | (b);

#define WRITE_RGB_PRESERVE_ALPHA(p, r, g, b) \
   *(p) = (*(p) & 0xff000000) | ((r) << 16) | ((g) << 8) | (b);

#define WRITE_RGBA(p, r, g, b, a) \
   *(p) = ((a) << 24) | ((r) << 16) | ((g) << 8) | (b);
 * **********************************************************************
*************************************************************************/

/* COPY OPS */

static void
__imlib_BlendRGBAToRGB(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
		       int w, int h, ImlibColorModifier *cm)
{
   LOOP_START_2

   READ_ALPHA(p1, a);
   if (a == 255)
      *p2 = *p1;	  
   else if (a > 0)
     {
	READ_RGB(p1,  r,  g, b );
	READ_RGB(p2, rr, gg, bb);

        BLEND_COLOR(a, nr, r, rr);
        BLEND_COLOR(a, ng, g, gg);
        BLEND_COLOR(a, nb, b, bb);

        WRITE_RGB_PRESERVE_ALPHA(p2, nr, ng, nb);
     }

   LOOP_END_WITH_INCREMENT
}

static void
__imlib_BlendRGBAToRGBA(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
			int w, int h, ImlibColorModifier *cm)
{
   LOOP_START_3

   READ_ALPHA(p1, a);
   if (a == 255)
      *p2 = *p1;	  
   else if (a > 0)
     {
	READ_RGB (p1,  r,  g, b );
	READ_RGBA(p2, rr, gg, bb, aa);

        BLEND_COLOR(a, nr, r, rr);
        BLEND_COLOR(a, ng, g, gg);
        BLEND_COLOR(a, nb, b, bb);
	SATURATE_UPPER(na, a + aa);

	WRITE_RGBA(p2, nr, ng, nb, na);
     }

   LOOP_END_WITH_INCREMENT
}

static void
__imlib_CopyRGBAToRGB(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
		      int w, int h, ImlibColorModifier *cm)
{
   LOOP_START

   *p2 = (*p2 & 0xff000000) | (*p1 & 0x00ffffff);

   LOOP_END_WITH_INCREMENT
}

static void
__imlib_CopyRGBAToRGBA(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
		       int w, int h, ImlibColorModifier *cm)
{
   /* FIXME: This could be a memcpy operation. */
   LOOP_START

   *p2 = *p1;

   LOOP_END_WITH_INCREMENT
}

/* ADD OPS */

static void
__imlib_AddBlendRGBAToRGB(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
			  int w, int h, ImlibColorModifier *cm)
{
   LOOP_START_2

   READ_ALPHA(p1, a);
   if (a > 0)
     {
	READ_RGB(p1,  r,  g, b );
	READ_RGB(p2, rr, gg, bb);

	ADD_COLOR_WITH_ALPHA(a, nr, r, rr);
	ADD_COLOR_WITH_ALPHA(a, ng, g, gg);
	ADD_COLOR_WITH_ALPHA(a, nb, b, bb);

	WRITE_RGB_PRESERVE_ALPHA(p2, nr, ng, nb);
     }

   LOOP_END_WITH_INCREMENT
}

static void
__imlib_AddBlendRGBAToRGBA(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
			   int w, int h, ImlibColorModifier *cm)
{
   LOOP_START_3

   READ_ALPHA(p1, a);
   if (a > 0)
     {
	READ_RGB (p1,  r,  g, b );
	READ_RGBA(p2, rr, gg, bb, aa);

	ADD_COLOR_WITH_ALPHA(a, nr, r, rr);
	ADD_COLOR_WITH_ALPHA(a, ng, g, gg);
	ADD_COLOR_WITH_ALPHA(a, nb, b, bb);
	SATURATE_UPPER(na, a + aa);

	WRITE_RGBA(p2, nr, ng, nb, na);
     }

   LOOP_END_WITH_INCREMENT
}

static void
__imlib_AddCopyRGBAToRGB(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
			 int w, int h, ImlibColorModifier *cm)
{
   LOOP_START_1

   READ_RGB(p1,  r,  g, b );
   READ_RGB(p2, rr, gg, bb);

   ADD_COLOR(nr, r, rr);
   ADD_COLOR(ng, g, gg);
   ADD_COLOR(nb, b, bb);

   WRITE_RGB_PRESERVE_ALPHA(p2, nr, ng, nb);

   LOOP_END_WITH_INCREMENT
}

static void
__imlib_AddCopyRGBAToRGBA(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
			  int w, int h, ImlibColorModifier *cm)
{
   LOOP_START_3

   READ_RGBA(p1,  r,  g, b , a );
   READ_RGBA(p2, rr, gg, bb, aa);

   ADD_COLOR(nr, r, rr);
   ADD_COLOR(ng, g, gg);
   ADD_COLOR(nb, b, bb);
   SATURATE_UPPER(na, a + aa);

   WRITE_RGBA(p2, nr, ng, nb, na);

   LOOP_END_WITH_INCREMENT
}

/* SUBTRACT OPS */

static void
__imlib_SubBlendRGBAToRGB(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
			  int w, int h, ImlibColorModifier *cm)
{
   LOOP_START_2

   READ_ALPHA(p1, a);
   if (a > 0)
     {
	READ_RGB(p1,  r,  g, b );
	READ_RGB(p2, rr, gg, bb);

	SUB_COLOR_WITH_ALPHA(a, nr, r, rr);
	SUB_COLOR_WITH_ALPHA(a, ng, g, gg);
	SUB_COLOR_WITH_ALPHA(a, nb, b, bb);

	WRITE_RGB_PRESERVE_ALPHA(p2, nr, ng, nb);
     }

   LOOP_END_WITH_INCREMENT
}

static void
__imlib_SubBlendRGBAToRGBA(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
			   int w, int h, ImlibColorModifier *cm)
{
   LOOP_START_3

   READ_ALPHA(p1, a);
   if (a > 0)
     {
	READ_RGB (p1,  r,  g, b );
	READ_RGBA(p2, rr, gg, bb, aa);

	SUB_COLOR_WITH_ALPHA(a, nr, r, rr);
	SUB_COLOR_WITH_ALPHA(a, ng, g, gg);
	SUB_COLOR_WITH_ALPHA(a, nb, b, bb);
	SATURATE_UPPER(na, a + aa);

	WRITE_RGBA(p2, nr, ng, nb, na);
     }

   LOOP_END_WITH_INCREMENT
}

static void
__imlib_SubCopyRGBAToRGB(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
			 int w, int h, ImlibColorModifier *cm)
{
   LOOP_START_1

   READ_RGB(p1,  r,  g, b );
   READ_RGB(p2, rr, gg, bb);

   SUB_COLOR(nr, r, rr);
   SUB_COLOR(ng, g, gg);
   SUB_COLOR(nb, b, bb);

   WRITE_RGB_PRESERVE_ALPHA(p2, nr, ng, nb);

   LOOP_END_WITH_INCREMENT
}

static void
__imlib_SubCopyRGBAToRGBA(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
			  int w, int h, ImlibColorModifier *cm)
{
   LOOP_START_3

   READ_RGBA(p1,  r,  g, b , a );
   READ_RGBA(p2, rr, gg, bb, aa);

   SUB_COLOR(nr, r, rr);
   SUB_COLOR(ng, g, gg);
   SUB_COLOR(nb, b, bb);
   SATURATE_UPPER(na, a + aa);

   WRITE_RGBA(p2, nr, ng, nb, na);

   LOOP_END_WITH_INCREMENT
}


/* RESHADE OPS */

static void
__imlib_ReBlendRGBAToRGB(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
			 int w, int h, ImlibColorModifier *cm)
{
   LOOP_START_2

   READ_ALPHA(p1, a);
   if (a > 0)
     {
	READ_RGB(p1,  r,  g, b );
	READ_RGB(p2, rr, gg, bb);

	RESHADE_COLOR_WITH_ALPHA(a, nr, r, rr);
	RESHADE_COLOR_WITH_ALPHA(a, ng, g, gg);
	RESHADE_COLOR_WITH_ALPHA(a, nb, b, bb);

	WRITE_RGB_PRESERVE_ALPHA(p2, nr, ng, nb);
     }

   LOOP_END_WITH_INCREMENT
}

static void
__imlib_ReBlendRGBAToRGBA(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
			  int w, int h, ImlibColorModifier *cm)
{
   LOOP_START_3

   READ_ALPHA(p1, a);
   if (a > 0)
     {
	READ_RGB (p1,  r,  g, b );
	READ_RGBA(p2, rr, gg, bb, aa);

	RESHADE_COLOR_WITH_ALPHA(a, nr, r, rr);
	RESHADE_COLOR_WITH_ALPHA(a, ng, g, gg);
	RESHADE_COLOR_WITH_ALPHA(a, nb, b, bb);
	SATURATE_UPPER(na, a + aa);

	WRITE_RGBA(p2, nr, ng, nb, na);
     }

   LOOP_END_WITH_INCREMENT
}

static void
__imlib_ReCopyRGBAToRGB(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
			int w, int h, ImlibColorModifier *cm)
{
   LOOP_START_1

   READ_RGB(p1,  r,  g, b );
   READ_RGB(p2, rr, gg, bb);

   RESHADE_COLOR(nr, r, rr);
   RESHADE_COLOR(ng, g, gg);
   RESHADE_COLOR(nb, b, bb);

   WRITE_RGB_PRESERVE_ALPHA(p2, nr, ng, nb);

   LOOP_END_WITH_INCREMENT
}

static void
__imlib_ReCopyRGBAToRGBA(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
			 int w, int h, ImlibColorModifier *cm)
{
   LOOP_START_3

   READ_RGBA(p1,  r,  g, b , a );
   READ_RGBA(p2, rr, gg, bb, aa);

   RESHADE_COLOR(nr, r, rr);
   RESHADE_COLOR(ng, g, gg);
   RESHADE_COLOR(nb, b, bb);
   SATURATE_UPPER(na, a + aa);

   WRITE_RGBA(p2, nr, ng, nb, na);

   LOOP_END_WITH_INCREMENT
}














/* COLORMOD COPY OPS */

static void
__imlib_BlendRGBAToRGBCmod(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
			int w, int h, ImlibColorModifier *cm)
{
   LOOP_START_2

   READ_ALPHA(p1, a);
   CMOD_APPLY_A(cm, a);
   if (a == 255)
      *p2 = *p1;	  
   else if (a > 0)
     {
	READ_RGB (p1,  r,  g,  b);
	READ_RGB (p2, rr, gg, bb);

	CMOD_APPLY_RGB(cm, r, g, b);

        BLEND_COLOR(a, nr, r, rr);
        BLEND_COLOR(a, ng, g, gg);
        BLEND_COLOR(a, nb, b, bb);

        WRITE_RGB_PRESERVE_ALPHA(p2, nr, ng, nb);
     }

   LOOP_END_WITH_INCREMENT
}

static void
__imlib_BlendRGBAToRGBACmod(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
			int w, int h, ImlibColorModifier *cm)
{
   LOOP_START_3

   READ_ALPHA(p1, a);
   CMOD_APPLY_A(cm, a);
   if (a == 255)
      *p2 = *p1;	  
   else if (a > 0)
     {
	READ_RGB(p1,  r,  g,  b);
	READ_RGBA(p2, rr, gg, bb, aa);

	CMOD_APPLY_RGB(cm, r, g, b);

        BLEND_COLOR(a, nr, r, rr);
        BLEND_COLOR(a, ng, g, gg);
        BLEND_COLOR(a, nb, b, bb);
	SATURATE_UPPER(na, a + aa);

	WRITE_RGBA(p2, nr, ng, nb, na);
     }

   LOOP_END_WITH_INCREMENT
}

static void
__imlib_CopyRGBAToRGBCmod(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
			int w, int h, ImlibColorModifier *cm)
{
   DATA8 r, g, b;
   LOOP_START

   READ_RGB(p1, r, g, b);

   CMOD_APPLY_RGB(cm, r, g, b);

   WRITE_RGB_PRESERVE_ALPHA(p2, r, g, b);

   LOOP_END_WITH_INCREMENT
}

static void
__imlib_CopyRGBAToRGBACmod(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
			int w, int h, ImlibColorModifier *cm)
{
   DATA8 r, g, b, a;
   LOOP_START

   READ_RGBA(p1, r, g, b, a);

   CMOD_APPLY_RGBA(cm, r, g, b, a);

   WRITE_RGBA(p2, r, g, b, a);

   LOOP_END_WITH_INCREMENT
}

/* ADD OPS */

static void
__imlib_AddBlendRGBAToRGBCmod(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
			int w, int h, ImlibColorModifier *cm)
{
   LOOP_START_2

   READ_ALPHA(p1, a);
   CMOD_APPLY_A(cm, a);
   if (a > 0)
     {
	READ_RGB(p1,  r,  g, b );
	READ_RGB(p2, rr, gg, bb);

	CMOD_APPLY_RGB(cm, r, g, b);

	ADD_COLOR_WITH_ALPHA(a, nr, r, rr);
	ADD_COLOR_WITH_ALPHA(a, ng, g, gg);
	ADD_COLOR_WITH_ALPHA(a, nb, b, bb);

	WRITE_RGB_PRESERVE_ALPHA(p2, nr, ng, nb);
     }

   LOOP_END_WITH_INCREMENT
}

static void
__imlib_AddBlendRGBAToRGBACmod(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
			int w, int h, ImlibColorModifier *cm)
{
   LOOP_START_3

   READ_ALPHA(p1, a);
   CMOD_APPLY_A(cm, a);
   if (a > 0)
     {
	READ_RGB (p1,  r,  g, b );
	READ_RGBA(p2, rr, gg, bb, aa);

	CMOD_APPLY_RGB(cm, r, g, b);

	ADD_COLOR_WITH_ALPHA(a, nr, r, rr);
	ADD_COLOR_WITH_ALPHA(a, ng, g, gg);
	ADD_COLOR_WITH_ALPHA(a, nb, b, bb);
	SATURATE_UPPER(na, a + aa);

	WRITE_RGBA(p2, nr, ng, nb, na);
     }

   LOOP_END_WITH_INCREMENT
}

static void
__imlib_AddCopyRGBAToRGBCmod(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
			int w, int h, ImlibColorModifier *cm)
{
   LOOP_START_1

   READ_RGB(p1,  r,  g, b );
   READ_RGB(p2, rr, gg, bb);

   CMOD_APPLY_RGB(cm, r, g, b);

   ADD_COLOR(nr, r, rr);
   ADD_COLOR(ng, g, gg);
   ADD_COLOR(nb, b, bb);

   WRITE_RGB_PRESERVE_ALPHA(p2, nr, ng, nb);

   LOOP_END_WITH_INCREMENT
}

static void
__imlib_AddCopyRGBAToRGBACmod(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
			int w, int h, ImlibColorModifier *cm)
{
   LOOP_START_3

   READ_RGBA(p1,  r,  g,  b,  a);
   READ_RGBA(p2, rr, gg, bb, aa);

   CMOD_APPLY_RGBA(cm, r, g, b, a);

   ADD_COLOR(nr, r, rr);
   ADD_COLOR(ng, g, gg);
   ADD_COLOR(nb, b, bb);
   SATURATE_UPPER(na, a + aa);

   WRITE_RGBA(p2, nr, ng, nb, na);

   LOOP_END_WITH_INCREMENT
}

/* SUBTRACT OPS */

static void
__imlib_SubBlendRGBAToRGBCmod(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
			int w, int h, ImlibColorModifier *cm)
{
   LOOP_START_2

   READ_ALPHA(p1, a);
   CMOD_APPLY_A(cm, a);
   if (a > 0)
     {
	READ_RGB(p1,  r,  g,  b);
	READ_RGB(p2, rr, gg, bb);

	CMOD_APPLY_RGB(cm, r, g, b);

	SUB_COLOR_WITH_ALPHA(a, nr, r, rr);
	SUB_COLOR_WITH_ALPHA(a, ng, g, gg);
	SUB_COLOR_WITH_ALPHA(a, nb, b, bb);

	WRITE_RGB_PRESERVE_ALPHA(p2, nr, ng, nb);
     }

   LOOP_END_WITH_INCREMENT
}

static void
__imlib_SubBlendRGBAToRGBACmod(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
			int w, int h, ImlibColorModifier *cm)
{
   LOOP_START_3

   READ_ALPHA(p1, a);
   CMOD_APPLY_A(cm, a);
   if (a > 0)
     {
	READ_RGB (p1,  r,  g, b );
	READ_RGBA(p2, rr, gg, bb, aa);

	CMOD_APPLY_RGB(cm, r, g, b);

	SUB_COLOR_WITH_ALPHA(a, nr, r, rr);
	SUB_COLOR_WITH_ALPHA(a, ng, g, gg);
	SUB_COLOR_WITH_ALPHA(a, nb, b, bb);
	SATURATE_UPPER(na, a + aa);

	WRITE_RGBA(p2, nr, ng, nb, na);
     }

   LOOP_END_WITH_INCREMENT
}

static void
__imlib_SubCopyRGBAToRGBCmod(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
			int w, int h, ImlibColorModifier *cm)
{
   LOOP_START_1

   READ_RGB(p1,  r,  g, b );
   READ_RGB(p2, rr, gg, bb);

   CMOD_APPLY_RGB(cm, r, g, b);

   SUB_COLOR(nr, r, rr);
   SUB_COLOR(ng, g, gg);
   SUB_COLOR(nb, b, bb);

   WRITE_RGB_PRESERVE_ALPHA(p2, nr, ng, nb);

   LOOP_END_WITH_INCREMENT
}

static void
__imlib_SubCopyRGBAToRGBACmod(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
			int w, int h, ImlibColorModifier *cm)
{
   LOOP_START_3

   READ_RGBA(p1,  r,  g,  b,  a);
   READ_RGBA(p2, rr, gg, bb, aa);

   CMOD_APPLY_RGBA(cm, r, g, b, a);

   SUB_COLOR(nr, r, rr);
   SUB_COLOR(ng, g, gg);
   SUB_COLOR(nb, b, bb);
   SATURATE_UPPER(na, a + aa);

   WRITE_RGBA(p2, nr, ng, nb, na);

   LOOP_END_WITH_INCREMENT
}


/* RESHADE OPS */

static void
__imlib_ReBlendRGBAToRGBCmod(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
			int w, int h, ImlibColorModifier *cm)
{
   LOOP_START_2

   READ_ALPHA(p1, a);
   CMOD_APPLY_A(cm, a);
   if (a > 0)
     {
	READ_RGB(p1,  r,  g,  b);
	READ_RGB(p2, rr, gg, bb);

	CMOD_APPLY_RGB(cm, r, g, b);

	RESHADE_COLOR_WITH_ALPHA(a, nr, r, rr);
	RESHADE_COLOR_WITH_ALPHA(a, ng, g, gg);
	RESHADE_COLOR_WITH_ALPHA(a, nb, b, bb);

	WRITE_RGB_PRESERVE_ALPHA(p2, nr, ng, nb);
     }

   LOOP_END_WITH_INCREMENT
}

static void
__imlib_ReBlendRGBAToRGBACmod(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
			int w, int h, ImlibColorModifier *cm)
{
   LOOP_START_3

   READ_ALPHA(p1, a);
   CMOD_APPLY_A(cm, a);
   if (a > 0)
     {
	READ_RGB (p1,  r,  g,  b);
	READ_RGBA(p2, rr, gg, bb, aa);

	CMOD_APPLY_RGB(cm, r, g, b);

	RESHADE_COLOR_WITH_ALPHA(a, nr, r, rr);
	RESHADE_COLOR_WITH_ALPHA(a, ng, g, gg);
	RESHADE_COLOR_WITH_ALPHA(a, nb, b, bb);
	SATURATE_UPPER(na, a + aa);

	WRITE_RGBA(p2, nr, ng, nb, na);
     }

   LOOP_END_WITH_INCREMENT
}

static void
__imlib_ReCopyRGBAToRGBCmod(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
			int w, int h, ImlibColorModifier *cm)
{
   LOOP_START_1

   READ_RGB(p1,  r,  g,  b);
   READ_RGB(p2, rr, gg, bb);

   CMOD_APPLY_RGB(cm, r, g, b);

   RESHADE_COLOR(nr, r, rr);
   RESHADE_COLOR(ng, g, gg);
   RESHADE_COLOR(nb, b, bb);

   WRITE_RGB_PRESERVE_ALPHA(p2, nr, ng, nb);

   LOOP_END_WITH_INCREMENT
}

static void
__imlib_ReCopyRGBAToRGBACmod(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump, 
			int w, int h, ImlibColorModifier *cm)
{
   LOOP_START_3

   READ_RGBA(p1,  r,  g,  b,  a);
   READ_RGBA(p2, rr, gg, bb, aa);

   CMOD_APPLY_RGBA(cm, r, g, b, a);

   RESHADE_COLOR(nr, r, rr);
   RESHADE_COLOR(ng, g, gg);
   RESHADE_COLOR(nb, b, bb);
   SATURATE_UPPER(na, a + aa);

   WRITE_RGBA(p2, nr, ng, nb, na);

   LOOP_END_WITH_INCREMENT
}





ImlibBlendFunction
__imlib_GetBlendFunction(ImlibOp op, char blend, char merge_alpha,
			 ImlibColorModifier * cm)
{
   ImlibBlendFunction blender = NULL;

   if (cm)
     {
	switch(op)
	  {
	  case OP_COPY:
	     if (merge_alpha)
	       {
		  if (blend)
		     blender = __imlib_BlendRGBAToRGBACmod;
		  else
		     blender = __imlib_CopyRGBAToRGBACmod;
	       }
             else
	       {
		  if (blend)
		     blender = __imlib_BlendRGBAToRGBCmod;
		  else
		     blender = __imlib_CopyRGBAToRGBCmod;
	       }
	     break;
	  case OP_ADD:
	     if (merge_alpha)
	       {
		  if (blend)
		     blender = __imlib_AddBlendRGBAToRGBACmod;
		  else
		     blender = __imlib_AddCopyRGBAToRGBACmod;
	       }
             else
	       {
		  if (blend)
		     blender = __imlib_AddBlendRGBAToRGBCmod;
		  else
		     blender = __imlib_AddCopyRGBAToRGBCmod;
	       }
	     break;
	  case OP_SUBTRACT:
	     if (merge_alpha)
	       {
		  if (blend)
		     blender = __imlib_SubBlendRGBAToRGBACmod;
		  else
		     blender = __imlib_SubCopyRGBAToRGBACmod;
	       }
             else
	       {
		  if (blend)
		     blender = __imlib_SubBlendRGBAToRGBCmod;
		  else
		     blender = __imlib_SubCopyRGBAToRGBCmod;
	       }
	     break;
	  case OP_RESHADE:
	     if (merge_alpha)
	       {
		  if (blend)
		     blender = __imlib_ReBlendRGBAToRGBACmod;
		  else
		     blender = __imlib_ReCopyRGBAToRGBACmod;
	       }
             else
	       {
		  if (blend)
		     blender = __imlib_ReBlendRGBAToRGBCmod;
		  else
		     blender = __imlib_ReCopyRGBAToRGBCmod;
	       }
	     break;
	  default:
	     break;
	  }
     }
   else
     {
	switch(op)
	  {
	  case OP_COPY:
	     if (merge_alpha)
	       {
		  if (blend)
		     blender = __imlib_BlendRGBAToRGBA;
		  else
		     blender = __imlib_CopyRGBAToRGBA;
	       }
             else
	       {
		  if (blend)
		     blender = __imlib_BlendRGBAToRGB;
		  else
		     blender = __imlib_CopyRGBAToRGB;
	       }
	     break;
	  case OP_ADD:
	     if (merge_alpha)
	       {
		  if (blend)
		     blender = __imlib_AddBlendRGBAToRGBA;
		  else
		     blender = __imlib_AddCopyRGBAToRGBA;
	       }
             else
	       {
		  if (blend)
		     blender = __imlib_AddBlendRGBAToRGB;
		  else
		     blender = __imlib_AddCopyRGBAToRGB;
	       }
	     break;
	  case OP_SUBTRACT:
	     if (merge_alpha)
	       {
		  if (blend)
		     blender = __imlib_SubBlendRGBAToRGBA;
		  else
		     blender = __imlib_SubCopyRGBAToRGBA;
	       }
             else
	       {
		  if (blend)
		     blender = __imlib_SubBlendRGBAToRGB;
		  else
		     blender = __imlib_SubCopyRGBAToRGB;
	       }
	     break;
	  case OP_RESHADE:
	     if (merge_alpha)
	       {
		  if (blend)
		     blender = __imlib_ReBlendRGBAToRGBA;
		  else
		     blender = __imlib_ReCopyRGBAToRGBA;
	       }
             else
	       {
		  if (blend)
		     blender = __imlib_ReBlendRGBAToRGB;
		  else
		     blender = __imlib_ReCopyRGBAToRGB;
	       }
	     break;
	  default:
	     break;
	  }
     }

   return blender;
}

void
__imlib_BlendRGBAToData(DATA32 *src, int src_w, int src_h, DATA32 *dst, 
			int dst_w, int dst_h, int sx, int sy, int dx, int dy,
			int w, int h, char blend, char merge_alpha,
			ImlibColorModifier *cm, ImlibOp op)
{
   ImlibBlendFunction blender;

   if (sx < 0)
     {
	w += sx;
	dx -= sx;
	sx = 0;
     }
   if (sy < 0)
     {
	h += sy;
	dy -= sy;
	sy = 0;
     }
   if (dx < 0)
     {
	w += dx;
	sx -= dx;
	dx = 0;
     }
   if (dy < 0)
     {
	h += dy;
	sy -= dy;
	dy = 0;
     }
   if ((w <= 0) || (h <= 0))
      return;
   if ((sx + w) > src_w)
      w = src_w - sx;
   if ((sy + h) > src_h)
      h = src_h - sy;
   if ((dx + w) > dst_w)
      w = dst_w - dx;
   if ((dy + h) > dst_h)
      h = dst_h - dy;   
   if ((w <= 0) || (h <= 0))
      return;

   blender = __imlib_GetBlendFunction(op, blend, merge_alpha, cm);
   if (blender)
      blender(src + (sy * src_w) + sx, src_w - w, 
	      dst + (dy * dst_w) + dx, dst_w - w, w, h, cm);
}

#define LINESIZE 16
#define CLIP(x, y, w, h, xx, yy, ww, hh) \
if (x < (xx)) {w += (x - (xx)); x = (xx);} \
if (y < (yy)) {h += (y - (yy)); y = (yy);} \
if ((x + w) > ((xx) + (ww))) {w = (ww) - x;} \
if ((y + h) > ((yy) + (hh))) {h = (hh) - y;}

void
__imlib_BlendImageToImage(ImlibImage *im_src, ImlibImage *im_dst,
			  char aa, char blend, char merge_alpha, 
			  int ssx, int ssy, int ssw, int ssh,
			  int ddx, int ddy, int ddw, int ddh, 
			   ImlibColorModifier *cm, ImlibOp op)
{
   if ((!(im_src->data)) && (im_src->loader) && (im_src->loader->load))
      im_src->loader->load(im_src, NULL, 0, 1);
   if ((!(im_dst->data)) && (im_dst->loader) && (im_src->loader->load))
      im_dst->loader->load(im_dst, NULL, 0, 1);
   if (!im_src->data)
      return;
   if (!im_dst->data)
      return;
   
   if ((ssw == ddw) &&
       (ssh == ddh))
     {
	if (!IMAGE_HAS_ALPHA(im_src))
	   blend = 0;
	if (!IMAGE_HAS_ALPHA(im_dst))
	   merge_alpha = 0;

	__imlib_BlendRGBAToData(im_src->data, im_src->w, im_src->h,
				im_dst->data, im_dst->w, im_dst->h,
				ssx, ssy,
				ddx, ddy,
				ssw, ssh, blend, merge_alpha, cm, op);
     }
   else
     {
	DATA32  **ypoints = NULL;
	int      *xpoints = NULL;
	int      *yapoints = NULL;
	int      *xapoints = NULL;
	DATA32   *buf = NULL;
	int       sx, sy, sw, sh, dx, dy, dw, dh, dxx, dyy, scw, sch, y2, x2;
	int       psx, psy, psw, psh;
	char      xup = 0, yup = 0;
	int       y, h, hh;
	sx = ssx;
	sy = ssy;
	sw = ssw;
	sh = ssh;
	dx = ddx;
	dy = ddy;
	dw = ddw;
	dh = ddh;
	/* don't do anything if we have a 0 width or height image to render */
	/* if the input rect size < 0 don't render either */
	if ((dw <= 0) || (dh <= 0) || (sw <= 0) || (sh <= 0))
	   return;
	/* clip the source rect to be within the actual image */
	psx = sx;
	psy = sy;
	psw = sw;
	psh = sh;
	CLIP(sx, sy, sw, sh, 0, 0, im_src->w, im_src->h);
	/* clip output coords to clipped input coords */
	if (psx != sx)
	   dx += ((sx - psx) * ddw) / ssw;
	if (psy != sy)
	   dy += ((sy - psy) * ddh) / ssh;
	if (psw != sw)
	   dw = (dw * sw) / psw;
	if (psh != sh)
	   dh = (dh * sh) / psh;
	if ((dw <= 0) || (dh <= 0) || (sw <= 0) || (sh <= 0))
	   return;
	psx = dx;
	psy = dy;
	psw = dw;
	psh = dh;
	x2 = sx;
	y2 = sy;
	CLIP(dx, dy, dw, dh, 0, 0, im_dst->w, im_dst->h);
	if ((dw <= 0) || (dh <= 0) || (sw <= 0) || (sh <= 0))
	   return;
	if (psx != dx)
	   sx += ((dx - psx) * ssw) / ddw;
	if (psy != dy)
	   sy += ((dy - psy) * ssh) / ddh;
	if (psw != dw)
	   sw = (sw * dw) / psw;
	if (psh != dh)
	   sh = (sh * dh) / psh;
	dxx = dx - psx;
	dyy = dy - psy;
	dxx += (x2 * ddw) / ssw;
	dyy += (y2 * ddh) / ssh;
	
	/* do a second check to see if we now have invalid coords */
	/* don't do anything if we have a 0 width or height image to render */
	/* if the input rect size < 0 don't render either */
	if ((dw <= 0) || (dh <= 0) || (sw <= 0) || (sh <= 0))
	   return;
	/* calculate the scaling factors of width and height for a whole image */
	scw = (ddw * im_src->w) / ssw;
	sch = (ddh * im_src->h) / ssh;
	/* if we are scaling the image at all make a scaling buffer */
	if (!((sw == dw) && (sh == dh)))
	  {
	     /* need to calculate ypoitns and xpoints array */
	     ypoints = __imlib_CalcYPoints(im_src->data, im_src->w, im_src->h,
					   sch, im_src->border.top,
					   im_src->border.bottom);
	     if (!ypoints)
		return;
	     xpoints = __imlib_CalcXPoints(im_src->w, scw,
					   im_src->border.left,
					   im_src->border.right);
	     if (!xpoints)
	       {
		  free(ypoints);
		  return;
	       }
	     /* calculate aliasing counts */
	     if (aa)
	       {
		  yapoints = __imlib_CalcApoints(im_src->h, sch,
						 im_src->border.top,
						 im_src->border.bottom);
		  if (!yapoints)
		    {
		       free(ypoints);
		       free(xpoints);
		       return;
		    }
		  xapoints = __imlib_CalcApoints(im_src->w, scw,
						 im_src->border.left,
						 im_src->border.right);
		  if (!xapoints)
		    {
		       free(yapoints);
		       free(ypoints);
		       free(xpoints);
		       return;
		    }
	       }
	  }
	else
	  {
	     if (!IMAGE_HAS_ALPHA(im_src))
		blend = 0;
	     if (!IMAGE_HAS_ALPHA(im_dst))
		merge_alpha = 0;
	     __imlib_BlendRGBAToData(im_src->data, im_src->w, im_src->h,
				     im_dst->data, im_dst->w, im_dst->h,
				     ssx, ssy,
				     ddx, ddy,
				     ssw, ssh, blend, merge_alpha, cm, op);
	     return;
	  }
	/* if we are scaling the image at all make a scaling buffer */
	/* allocate a buffer to render scaled RGBA data into */
	buf = malloc(dw * LINESIZE * sizeof(DATA32));
	if (!buf)
	  {
	     if (aa)
	       {
		  free(xapoints);
		  free(yapoints);
	       }
	     free(ypoints);
	     free(xpoints);
	  }
	/* setup h */
	h = dh;
	/* set our scaling up in x / y dir flags */
	if (dw > sw)
	   xup = 1;
	if (dh > sh)
	   yup = 1;
	if (!IMAGE_HAS_ALPHA(im_src))
	   blend = 0;
	if (!IMAGE_HAS_ALPHA(im_dst))
	   merge_alpha = 0;
	/* scale in LINESIZE Y chunks and convert to depth*/
	for (y = 0; y < dh; y += LINESIZE)
	  {
	     hh = LINESIZE;
	     if (h < LINESIZE)
		hh = h;
	     /* scale the imagedata for this LINESIZE lines chunk of image */
	     if (aa)
	       {
		  if (IMAGE_HAS_ALPHA(im_src))
		     __imlib_ScaleAARGBA(ypoints, xpoints, buf, xapoints,
					 yapoints, xup, yup, dxx, dyy + y,
					 0, 0, dw, hh, dw, im_src->w);
		  else
		     __imlib_ScaleAARGB(ypoints, xpoints, buf, xapoints,
					yapoints, xup, yup, dxx, dyy + y,
					0, 0, dw, hh, dw, im_src->w);
	       }
	     else
		__imlib_ScaleSampleRGBA(ypoints, xpoints, buf, dxx, dyy + y,
					0, 0, dw, hh, dw);

	     __imlib_BlendRGBAToData(buf, dw, hh,
				     im_dst->data, im_dst->w,
				     im_dst->h,
				     0, 0, dx, dy + y, dw, dh,
				     blend, merge_alpha, cm, op);
	     h -= LINESIZE;
	  }
	/* free up our buffers and point tables */
	if (buf)
	  {
	     free(buf);
	     free(ypoints);
	     free(xpoints);
	  }
	if (aa)
	  {
	     free(yapoints);
	     free(xapoints);
	  }
     }
}
