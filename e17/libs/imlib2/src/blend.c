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
   DATA32 tmp;                                       \
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
	p1 += srcw - w;                              \
	p2 += dstw - w;                              \
     }

#define LOOP_END_WITH_INCREMENT                      \
   p1++;                                             \
   p2++;                                             \
   LOOP_END

/* COPY OPS */

static void
__imlib_BlendRGBAToRGB(DATA32 *src, int srcw, DATA32 *dst, int dstw, 
		       int w, int h, ImlibColorModifier *cm)
{
   LOOP_START_2

   a = A_VAL(p1);
   BLEND_COLOR(a, R_VAL(p2), R_VAL(p1), R_VAL(p2));
   BLEND_COLOR(a, G_VAL(p2), G_VAL(p1), G_VAL(p2));
   BLEND_COLOR(a, B_VAL(p2), B_VAL(p1), B_VAL(p2));
   
   LOOP_END_WITH_INCREMENT
}

static void
__imlib_BlendRGBAToRGBA(DATA32 *src, int srcw, DATA32 *dst, int dstw, 
			int w, int h, ImlibColorModifier *cm)
{
   LOOP_START_3

   SATURATE_UPPER(a, A_VAL(p1) + (255 - A_VAL(p2)));
   BLEND_COLOR(a, R_VAL(p2), R_VAL(p1), R_VAL(p2));
   BLEND_COLOR(a, G_VAL(p2), G_VAL(p1), G_VAL(p2));
   BLEND_COLOR(a, B_VAL(p2), B_VAL(p1), B_VAL(p2));
   SATURATE_UPPER(A_VAL(p2), A_VAL(p1) + A_VAL(p2));

   LOOP_END_WITH_INCREMENT
}

static void
__imlib_BlendRGBToRGBA(DATA32 *src, int srcw, DATA32 *dst, int dstw, 
			int w, int h, ImlibColorModifier *cm)
{
   LOOP_START

   *p2 = 0xff000000 | (*p1 & 0x00ffffff);

   LOOP_END_WITH_INCREMENT
}

static void
__imlib_CopyRGBAToRGB(DATA32 *src, int srcw, DATA32 *dst, int dstw, 
		      int w, int h, ImlibColorModifier *cm)
{
   LOOP_START

   *p2 = (*p2 & 0xff000000) | (*p1 & 0x00ffffff);

   LOOP_END_WITH_INCREMENT
}

static void
__imlib_CopyRGBToRGBA(DATA32 *src, int srcw, DATA32 *dst, int dstw, 
		      int w, int h, ImlibColorModifier *cm)
{
   LOOP_START

   *p2 = 0xff000000 | (*p1 & 0x00ffffff);

   LOOP_END_WITH_INCREMENT
}

static void
__imlib_CopyRGBAToRGBA(DATA32 *src, int srcw, DATA32 *dst, int dstw, 
		       int w, int h, ImlibColorModifier *cm)
{
   /* FIXME: This could be a memcpy operation. */
   LOOP_START

   *p2 = *p1;

   LOOP_END_WITH_INCREMENT
}

/* ADD OPS */

static void
__imlib_AddBlendRGBAToRGB(DATA32 *src, int srcw, DATA32 *dst, int dstw, 
			  int w, int h, ImlibColorModifier *cm)
{
   LOOP_START_2

   a = A_VAL(p1);
   ADD_COLOR_WITH_ALPHA(a, R_VAL(p2), R_VAL(p1), R_VAL(p2));
   ADD_COLOR_WITH_ALPHA(a, G_VAL(p2), G_VAL(p1), G_VAL(p2));
   ADD_COLOR_WITH_ALPHA(a, B_VAL(p2), B_VAL(p1), B_VAL(p2));

   LOOP_END_WITH_INCREMENT
}

static void
__imlib_AddBlendRGBAToRGBA(DATA32 *src, int srcw, DATA32 *dst, int dstw, 
			   int w, int h, ImlibColorModifier *cm)
{
   LOOP_START_3

   SATURATE_UPPER(a, A_VAL(p1) + (255 - A_VAL(p2)));
   ADD_COLOR_WITH_ALPHA(a, R_VAL(p2), R_VAL(p1), R_VAL(p2));
   ADD_COLOR_WITH_ALPHA(a, G_VAL(p2), G_VAL(p1), G_VAL(p2));
   ADD_COLOR_WITH_ALPHA(a, B_VAL(p2), B_VAL(p1), B_VAL(p2));
   SATURATE_UPPER(A_VAL(p2), A_VAL(p1) + A_VAL(p2));

   LOOP_END_WITH_INCREMENT
}

static void
__imlib_AddCopyRGBAToRGB(DATA32 *src, int srcw, DATA32 *dst, int dstw, 
			 int w, int h, ImlibColorModifier *cm)
{
   LOOP_START_1

   ADD_COLOR(R_VAL(p2), R_VAL(p1), R_VAL(p2));
   ADD_COLOR(G_VAL(p2), G_VAL(p1), G_VAL(p2));
   ADD_COLOR(B_VAL(p2), B_VAL(p1), B_VAL(p2));

   LOOP_END_WITH_INCREMENT
}

static void
__imlib_AddCopyRGBAToRGBA(DATA32 *src, int srcw, DATA32 *dst, int dstw, 
			  int w, int h, ImlibColorModifier *cm)
{
   LOOP_START_3

   SATURATE_UPPER(a, A_VAL(p1) + (255 - A_VAL(p2)));
   ADD_COLOR(R_VAL(p2), R_VAL(p1), R_VAL(p2));
   ADD_COLOR(G_VAL(p2), G_VAL(p1), G_VAL(p2));
   ADD_COLOR(B_VAL(p2), B_VAL(p1), B_VAL(p2));
   SATURATE_UPPER(A_VAL(p2), A_VAL(p1) + A_VAL(p2));

   LOOP_END_WITH_INCREMENT
}

/* SUBTRACT OPS */

static void
__imlib_SubBlendRGBAToRGB(DATA32 *src, int srcw, DATA32 *dst, int dstw, 
			  int w, int h, ImlibColorModifier *cm)
{
   LOOP_START_2

   SATURATE_UPPER(a, A_VAL(p1) + (255 - A_VAL(p2)));
   SUB_COLOR_WITH_ALPHA(a, R_VAL(p2), R_VAL(p1), R_VAL(p2));
   SUB_COLOR_WITH_ALPHA(a, G_VAL(p2), G_VAL(p1), G_VAL(p2));
   SUB_COLOR_WITH_ALPHA(a, B_VAL(p2), B_VAL(p1), B_VAL(p2));
   SATURATE_UPPER(A_VAL(p2), A_VAL(p1) + A_VAL(p2));

   LOOP_END_WITH_INCREMENT
}

static void
__imlib_SubBlendRGBAToRGBA(DATA32 *src, int srcw, DATA32 *dst, int dstw, 
			   int w, int h, ImlibColorModifier *cm)
{
   LOOP_START_3

   SATURATE_UPPER(a, A_VAL(p1) + (255 - A_VAL(p2)));
   SUB_COLOR_WITH_ALPHA(a, R_VAL(p2), R_VAL(p1), R_VAL(p2));
   SUB_COLOR_WITH_ALPHA(a, G_VAL(p2), G_VAL(p1), G_VAL(p2));
   SUB_COLOR_WITH_ALPHA(a, B_VAL(p2), B_VAL(p1), B_VAL(p2));
   SATURATE_UPPER(A_VAL(p2), A_VAL(p1) + A_VAL(p2));

   LOOP_END_WITH_INCREMENT
}

static void
__imlib_SubCopyRGBAToRGB(DATA32 *src, int srcw, DATA32 *dst, int dstw, 
			 int w, int h, ImlibColorModifier *cm)
{
   LOOP_START_1

   SUB_COLOR(R_VAL(p2), R_VAL(p1), R_VAL(p2));
   SUB_COLOR(G_VAL(p2), G_VAL(p1), G_VAL(p2));
   SUB_COLOR(B_VAL(p2), B_VAL(p1), B_VAL(p2));

   LOOP_END_WITH_INCREMENT
}

static void
__imlib_SubCopyRGBAToRGBA(DATA32 *src, int srcw, DATA32 *dst, int dstw, 
			  int w, int h, ImlibColorModifier *cm)
{
   LOOP_START_3

   SUB_COLOR(R_VAL(p2), R_VAL(p1), R_VAL(p2));
   SUB_COLOR(G_VAL(p2), G_VAL(p1), G_VAL(p2));
   SUB_COLOR(B_VAL(p2), B_VAL(p1), B_VAL(p2));
   SATURATE_UPPER(A_VAL(p2), A_VAL(p1) + A_VAL(p2));

   LOOP_END_WITH_INCREMENT
}


/* RESHADE OPS */

static void
__imlib_ReBlendRGBAToRGB(DATA32 *src, int srcw, DATA32 *dst, int dstw, 
			 int w, int h, ImlibColorModifier *cm)
{
   LOOP_START_2

   a = A_VAL(p1);

   RESHADE_COLOR_WITH_ALPHA(a, R_VAL(p2), R_VAL(p1), R_VAL(p2));
   RESHADE_COLOR_WITH_ALPHA(a, G_VAL(p2), G_VAL(p1), G_VAL(p2));
   RESHADE_COLOR_WITH_ALPHA(a, B_VAL(p2), B_VAL(p1), B_VAL(p2));

   LOOP_END_WITH_INCREMENT
}

static void
__imlib_ReBlendRGBAToRGBA(DATA32 *src, int srcw, DATA32 *dst, int dstw, 
			  int w, int h, ImlibColorModifier *cm)
{
   LOOP_START_3

   SATURATE_UPPER(a, A_VAL(p1) + (255 - A_VAL(p2)));
   RESHADE_COLOR_WITH_ALPHA(a, R_VAL(p2), R_VAL(p1), R_VAL(p2));
   RESHADE_COLOR_WITH_ALPHA(a, G_VAL(p2), G_VAL(p1), G_VAL(p2));
   RESHADE_COLOR_WITH_ALPHA(a, B_VAL(p2), B_VAL(p1), B_VAL(p2));
   SATURATE_UPPER(A_VAL(p2), A_VAL(p1) + A_VAL(p2));

   LOOP_END_WITH_INCREMENT
}

static void
__imlib_ReCopyRGBAToRGB(DATA32 *src, int srcw, DATA32 *dst, int dstw, 
			int w, int h, ImlibColorModifier *cm)
{
   LOOP_START_1

   RESHADE_COLOR(R_VAL(p2), R_VAL(p1), R_VAL(p2));
   RESHADE_COLOR(G_VAL(p2), G_VAL(p1), G_VAL(p2));
   RESHADE_COLOR(B_VAL(p2), B_VAL(p1), B_VAL(p2));

   LOOP_END_WITH_INCREMENT
}

static void
__imlib_ReCopyRGBAToRGBA(DATA32 *src, int srcw, DATA32 *dst, int dstw, 
			 int w, int h, ImlibColorModifier *cm)
{
   LOOP_START_3

   RESHADE_COLOR(R_VAL(p2), R_VAL(p1), R_VAL(p2));
   RESHADE_COLOR(G_VAL(p2), G_VAL(p1), G_VAL(p2));
   RESHADE_COLOR(B_VAL(p2), B_VAL(p1), B_VAL(p2));
   SATURATE_UPPER(A_VAL(p2), A_VAL(p1) + A_VAL(p2));

   LOOP_END_WITH_INCREMENT
}


















/* WITH COLOMOD */
/* COPY OPS */

static void
__imlib_BlendRGBAToRGBCmod(DATA32 *src, int srcw, DATA32 *dst, int dstw, 
		       int w, int h, ImlibColorModifier *cm)
{
   LOOP_START_2

   a = A_CMOD(cm, A_VAL(p1));
   BLEND_COLOR(a, R_VAL(p2), R_CMOD(cm, R_VAL(p1)), R_VAL(p2));
   BLEND_COLOR(a, G_VAL(p2), G_CMOD(cm, G_VAL(p1)), G_VAL(p2));
   BLEND_COLOR(a, B_VAL(p2), B_CMOD(cm, B_VAL(p1)), B_VAL(p2));

   LOOP_END_WITH_INCREMENT
}

static void
__imlib_BlendRGBAToRGBACmod(DATA32 *src, int srcw, DATA32 *dst, int dstw, 
			int w, int h, ImlibColorModifier *cm)
{
   LOOP_START_3

   SATURATE_UPPER(a, A_CMOD(cm, A_VAL(p1)) + (255 - A_VAL(p2)));
   BLEND_COLOR(a, R_VAL(p2), R_CMOD(cm, R_VAL(p1)), R_VAL(p2));
   BLEND_COLOR(a, G_VAL(p2), G_CMOD(cm, G_VAL(p1)), G_VAL(p2));
   BLEND_COLOR(a, B_VAL(p2), B_CMOD(cm, B_VAL(p1)), B_VAL(p2));
   SATURATE_UPPER(A_VAL(p2), A_CMOD(cm, A_VAL(p1)) + A_VAL(p2));

   LOOP_END_WITH_INCREMENT
}

static void
__imlib_BlendRGBToRGBACmod(DATA32 *src, int srcw, DATA32 *dst, int dstw, 
			int w, int h, ImlibColorModifier *cm)
{
   LOOP_START_3

   R_VAL(p2) = R_CMOD(cm, R_VAL(p1));
   G_VAL(p2) = G_CMOD(cm, G_VAL(p1));
   B_VAL(p2) = B_CMOD(cm, B_VAL(p1));
   A_VAL(p2) = 0xff;

   LOOP_END_WITH_INCREMENT
}

static void
__imlib_CopyRGBAToRGBCmod(DATA32 *src, int srcw, DATA32 *dst, int dstw, 
		      int w, int h, ImlibColorModifier *cm)
{
   LOOP_START

   R_VAL(p2) = R_CMOD(cm, R_VAL(p1));
   G_VAL(p2) = G_CMOD(cm, G_VAL(p1));
   B_VAL(p2) = B_CMOD(cm, B_VAL(p1));

   LOOP_END_WITH_INCREMENT
}

static void
__imlib_CopyRGBToRGBACmod(DATA32 *src, int srcw, DATA32 *dst, int dstw, 
		      int w, int h, ImlibColorModifier *cm)
{
   LOOP_START

   R_VAL(p2) = R_CMOD(cm, R_VAL(p1));
   G_VAL(p2) = G_CMOD(cm, G_VAL(p1));
   B_VAL(p2) = B_CMOD(cm, B_VAL(p1));
   A_VAL(p2) = 0xff;

   LOOP_END_WITH_INCREMENT
}

static void
__imlib_CopyRGBAToRGBACmod(DATA32 *src, int srcw, DATA32 *dst, int dstw, 
		       int w, int h, ImlibColorModifier *cm)
{
   LOOP_START

   R_VAL(p2) = R_CMOD(cm, R_VAL(p1));
   G_VAL(p2) = G_CMOD(cm, G_VAL(p1));
   B_VAL(p2) = B_CMOD(cm, B_VAL(p1));
   A_VAL(p2) = A_CMOD(cm, A_VAL(p1));

   LOOP_END_WITH_INCREMENT
}

/* ADD OPS */

static void
__imlib_AddBlendRGBAToRGBCmod(DATA32 *src, int srcw, DATA32 *dst, int dstw, 
			  int w, int h, ImlibColorModifier *cm)
{
   LOOP_START_2

   SATURATE_UPPER(a, A_CMOD(cm, A_VAL(p1)) + (255 - A_VAL(p2)));
   ADD_COLOR_WITH_ALPHA(a, R_VAL(p2), R_CMOD(cm, R_VAL(p1)), R_VAL(p2));
   ADD_COLOR_WITH_ALPHA(a, G_VAL(p2), G_CMOD(cm, G_VAL(p1)), G_VAL(p2));
   ADD_COLOR_WITH_ALPHA(a, B_VAL(p2), B_CMOD(cm, B_VAL(p1)), B_VAL(p2));

   LOOP_END_WITH_INCREMENT
}

static void
__imlib_AddBlendRGBAToRGBACmod(DATA32 *src, int srcw, DATA32 *dst, int dstw, 
			   int w, int h, ImlibColorModifier *cm)
{
   LOOP_START_3

   SATURATE_UPPER(a, A_CMOD(cm, A_VAL(p1)) + (255 - A_VAL(p2)));
   ADD_COLOR_WITH_ALPHA(a, R_VAL(p2), R_CMOD(cm, R_VAL(p1)), R_VAL(p2));
   ADD_COLOR_WITH_ALPHA(a, G_VAL(p2), G_CMOD(cm, G_VAL(p1)), G_VAL(p2));
   ADD_COLOR_WITH_ALPHA(a, B_VAL(p2), B_CMOD(cm, B_VAL(p1)), B_VAL(p2));
   SATURATE_UPPER(A_VAL(p2), A_CMOD(cm, A_VAL(p1)) + A_VAL(p2));

   LOOP_END_WITH_INCREMENT
}

static void
__imlib_AddCopyRGBAToRGBCmod(DATA32 *src, int srcw, DATA32 *dst, int dstw, 
			 int w, int h, ImlibColorModifier *cm)
{
   LOOP_START_1

   ADD_COLOR(R_VAL(p2), R_CMOD(cm, R_VAL(p1)), R_VAL(p2));
   ADD_COLOR(G_VAL(p2), G_CMOD(cm, G_VAL(p1)), G_VAL(p2));
   ADD_COLOR(B_VAL(p2), B_CMOD(cm, B_VAL(p1)), B_VAL(p2));

   LOOP_END_WITH_INCREMENT
}

static void
__imlib_AddCopyRGBAToRGBACmod(DATA32 *src, int srcw, DATA32 *dst, int dstw, 
			  int w, int h, ImlibColorModifier *cm)
{
   LOOP_START_3

   ADD_COLOR(R_VAL(p2), R_CMOD(cm, R_VAL(p1)), R_VAL(p2));
   ADD_COLOR(G_VAL(p2), G_CMOD(cm, G_VAL(p1)), G_VAL(p2));
   ADD_COLOR(B_VAL(p2), B_CMOD(cm, B_VAL(p1)), B_VAL(p2));
   SATURATE_UPPER(A_VAL(p2), A_CMOD(cm, A_VAL(p1)) + A_VAL(p2));

   LOOP_END_WITH_INCREMENT
}

/* SUBTRACT OPS */

static void
__imlib_SubBlendRGBAToRGBCmod(DATA32 *src, int srcw, DATA32 *dst, int dstw, 
			  int w, int h, ImlibColorModifier *cm)
{
   LOOP_START_2

   a = A_CMOD(cm, A_VAL(p1));

   SUB_COLOR_WITH_ALPHA(a, R_VAL(p2), R_CMOD(cm, R_VAL(p1)), R_VAL(p2));
   SUB_COLOR_WITH_ALPHA(a, G_VAL(p2), G_CMOD(cm, G_VAL(p1)), G_VAL(p2));
   SUB_COLOR_WITH_ALPHA(a, B_VAL(p2), B_CMOD(cm, B_VAL(p1)), B_VAL(p2));

   LOOP_END_WITH_INCREMENT
}

static void
__imlib_SubBlendRGBAToRGBACmod(DATA32 *src, int srcw, DATA32 *dst, int dstw, 
			   int w, int h, ImlibColorModifier *cm)
{
   LOOP_START_3

   SATURATE_UPPER(a, A_CMOD(cm, A_VAL(p1)) + (255 - A_VAL(p2)));
   SUB_COLOR_WITH_ALPHA(a, R_VAL(p2), R_CMOD(cm, R_VAL(p1)), R_VAL(p2));
   SUB_COLOR_WITH_ALPHA(a, G_VAL(p2), G_CMOD(cm, G_VAL(p1)), G_VAL(p2));
   SUB_COLOR_WITH_ALPHA(a, B_VAL(p2), B_CMOD(cm, B_VAL(p1)), B_VAL(p2));
   SATURATE_UPPER(A_VAL(p2), A_CMOD(cm, A_VAL(p1)) + A_VAL(p2));

   LOOP_END_WITH_INCREMENT
}

static void
__imlib_SubCopyRGBAToRGBCmod(DATA32 *src, int srcw, DATA32 *dst, int dstw, 
			 int w, int h, ImlibColorModifier *cm)
{
   LOOP_START_1

   SUB_COLOR(R_VAL(p2), R_CMOD(cm, R_VAL(p1)), R_VAL(p2));
   SUB_COLOR(G_VAL(p2), G_CMOD(cm, G_VAL(p1)), G_VAL(p2));
   SUB_COLOR(B_VAL(p2), B_CMOD(cm, B_VAL(p1)), B_VAL(p2));

   LOOP_END_WITH_INCREMENT
}

static void
__imlib_SubCopyRGBAToRGBACmod(DATA32 *src, int srcw, DATA32 *dst, int dstw, 
			  int w, int h, ImlibColorModifier *cm)
{
   LOOP_START_3

   SUB_COLOR(R_VAL(p2), R_CMOD(cm, R_VAL(p1)), R_VAL(p2));
   SUB_COLOR(G_VAL(p2), G_CMOD(cm, G_VAL(p1)), G_VAL(p2));
   SUB_COLOR(B_VAL(p2), B_CMOD(cm, B_VAL(p1)), B_VAL(p2));
   SATURATE_UPPER(A_VAL(p2), A_CMOD(cm, A_VAL(p1)) + A_VAL(p2));

   LOOP_END_WITH_INCREMENT
}


/* RESHADE OPS */

static void
__imlib_ReBlendRGBAToRGBCmod(DATA32 *src, int srcw, DATA32 *dst, int dstw, 
			 int w, int h, ImlibColorModifier *cm)
{
   LOOP_START_2
   
   a = A_CMOD(cm, A_VAL(p1));

   RESHADE_COLOR_WITH_ALPHA(a, R_VAL(p2), R_CMOD(cm, R_VAL(p1)), R_VAL(p2));
   RESHADE_COLOR_WITH_ALPHA(a, G_VAL(p2), G_CMOD(cm, G_VAL(p1)), G_VAL(p2));
   RESHADE_COLOR_WITH_ALPHA(a, B_VAL(p2), B_CMOD(cm, B_VAL(p1)), B_VAL(p2));

   LOOP_END_WITH_INCREMENT
}

static void
__imlib_ReBlendRGBAToRGBACmod(DATA32 *src, int srcw, DATA32 *dst, int dstw, 
			  int w, int h, ImlibColorModifier *cm)
{
   LOOP_START_3

   SATURATE_UPPER(a, A_CMOD(cm, A_VAL(p1)) + (255 - A_VAL(p2)));
   RESHADE_COLOR_WITH_ALPHA(a, R_VAL(p2), R_CMOD(cm, R_VAL(p1)), R_VAL(p2));
   RESHADE_COLOR_WITH_ALPHA(a, G_VAL(p2), G_CMOD(cm, G_VAL(p1)), G_VAL(p2));
   RESHADE_COLOR_WITH_ALPHA(a, B_VAL(p2), B_CMOD(cm, B_VAL(p1)), B_VAL(p2));
   SATURATE_UPPER(A_VAL(p2), A_CMOD(cm, A_VAL(p1)) + A_VAL(p2));

   LOOP_END_WITH_INCREMENT
}

static void
__imlib_ReCopyRGBAToRGBCmod(DATA32 *src, int srcw, DATA32 *dst, int dstw, 
			int w, int h, ImlibColorModifier *cm)
{
   LOOP_START_1

   RESHADE_COLOR(R_VAL(p2), R_CMOD(cm, R_VAL(p1)), R_VAL(p2));
   RESHADE_COLOR(G_VAL(p2), G_CMOD(cm, G_VAL(p1)), G_VAL(p2));
   RESHADE_COLOR(B_VAL(p2), B_CMOD(cm, B_VAL(p1)), B_VAL(p2));

   LOOP_END_WITH_INCREMENT
}

static void
__imlib_ReCopyRGBAToRGBACmod(DATA32 *src, int srcw, DATA32 *dst, int dstw, 
			 int w, int h, ImlibColorModifier *cm)
{
   LOOP_START_3

   RESHADE_COLOR(R_VAL(p2), R_CMOD(cm, R_VAL(p1)), R_VAL(p2));
   RESHADE_COLOR(G_VAL(p2), G_CMOD(cm, G_VAL(p1)), G_VAL(p2));
   RESHADE_COLOR(B_VAL(p2), B_CMOD(cm, B_VAL(p1)), B_VAL(p2));
   SATURATE_UPPER(A_VAL(p2), A_CMOD(cm, A_VAL(p1)) + A_VAL(p2));

   LOOP_END_WITH_INCREMENT
}








ImlibBlendFunction
__imlib_GetBlendFunction(ImlibOp op, char blend, char merge_alpha, char rgb_src,
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
		  if (rgb_src)
		    {
		       if (blend)
			  blender = __imlib_BlendRGBToRGBACmod;
		       else
			  blender = __imlib_CopyRGBAToRGBACmod;
		    }
		  else
		    {
		       if (blend)
			  blender = __imlib_BlendRGBAToRGBACmod;
		       else
			  blender = __imlib_CopyRGBAToRGBACmod;
		    }
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
#ifdef DO_MMX_ASM
	if (__imlib_get_cpuid() & CPUID_MMX)
	  {
	     switch(op)
	       {
	       case OP_COPY:
		  if (merge_alpha)
		    {
		       if (rgb_src)
			 {
			    blender = __imlib_mmx_copy_rgb_to_rgba;
			 }
		       else
			 {
			    if (blend)
			       blender = __imlib_mmx_blend_rgba_to_rgba;
			    else
			       blender = __imlib_mmx_copy_rgba_to_rgba;
			 }
		    }
		  else
		    {
		       if (blend)
			  blender = __imlib_mmx_blend_rgba_to_rgb;
		       else
			  blender = __imlib_mmx_copy_rgba_to_rgb;
		    }
		  break;
	       case OP_ADD:
		  if (merge_alpha)
		    {
		       if (blend)
			  blender = __imlib_mmx_add_blend_rgba_to_rgba;
		       else
			  blender = __imlib_mmx_add_copy_rgba_to_rgba;
		    }
		  else
		    {
		       if (blend)
			  blender = __imlib_mmx_add_blend_rgba_to_rgb;
		       else
			  blender = __imlib_mmx_add_copy_rgba_to_rgb;
		    }
		  break;
	       case OP_SUBTRACT:
		  if (merge_alpha)
		    {
		       if (blend)
			  blender = __imlib_mmx_subtract_blend_rgba_to_rgba;
		       else
			  blender = __imlib_mmx_subtract_copy_rgba_to_rgba;
		    }
		  else
		    {
		       if (blend)
			  blender = __imlib_mmx_subtract_blend_rgba_to_rgb;
		       else
			  blender = __imlib_mmx_subtract_copy_rgba_to_rgb;
		    }
		  break;
	       case OP_RESHADE:
		  if (merge_alpha)
		    {
		       if (blend)
			  blender = __imlib_mmx_reshade_blend_rgba_to_rgba;
		       else
			  blender = __imlib_mmx_reshade_copy_rgba_to_rgba;
		    }
		  else
		    {
		       if (blend)
			  blender = __imlib_mmx_reshade_blend_rgba_to_rgb;
		       else
			  blender = __imlib_mmx_reshade_copy_rgba_to_rgb;
		    }
		  break;
	       default:
		  break;
	       }
	  }
	else
#endif
	  {
	     switch(op)
	       {
	       case OP_COPY:
		  if (merge_alpha)
		    {
		       if (rgb_src)
			 {
			    if (blend)
			      {
				 blender = __imlib_BlendRGBToRGBA;
			      }
			    else
			       blender = __imlib_CopyRGBAToRGBA;
			 }
		       else
			 {
			    if (blend)
			       blender = __imlib_BlendRGBAToRGBA;
			    else
			       blender = __imlib_CopyRGBAToRGBA;
			 }
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
     }

   return blender;
}

void
__imlib_BlendRGBAToData(DATA32 *src, int src_w, int src_h, DATA32 *dst, 
			int dst_w, int dst_h, int sx, int sy, int dx, int dy,
			int w, int h, char blend, char merge_alpha,
			ImlibColorModifier *cm, ImlibOp op, char rgb_src)
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
   
   blender = __imlib_GetBlendFunction(op, blend, merge_alpha, rgb_src, cm);
   if (blender)
      blender(src + (sy * src_w) + sx, src_w, 
	      dst + (dy * dst_w) + dx, dst_w, w, h, cm);
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
   char rgb_src = 0;
   
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
	if (!IMAGE_HAS_ALPHA(im_dst))
	   merge_alpha = 0;
	if (!IMAGE_HAS_ALPHA(im_src))
	  {
	     rgb_src = 1;
	     if (merge_alpha)
		blend = 1;
	     else
		blend = 0;
	  }

	__imlib_BlendRGBAToData(im_src->data, im_src->w, im_src->h,
				im_dst->data, im_dst->w, im_dst->h,
				ssx, ssy,
				ddx, ddy,
				ssw, ssh, blend, merge_alpha, cm, op, rgb_src);
     }
   else
     {
	ImlibScaleInfo *scaleinfo = NULL;
	DATA32   *buf = NULL;
	int       sx, sy, sw, sh, dx, dy, dw, dh, dxx, dyy, y2, x2;
	int       psx, psy, psw, psh;
	int       y, h, hh;
	int       do_mmx;
	sx = ssx;
	sy = ssy;
	sw = ssw;
	sh = ssh;
	dx = ddx;
	dy = ddy;
	dw = abs(ddw);
	dh = abs(ddh);
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
	if (psx != sx)
	   dx += ((sx - psx) * abs(ddw)) / ssw;
	if (psy != sy)
	   dy += ((sy - psy) * abs(ddh)) / ssh;
	if (psw != sw)
	   dw = (dw * sw) / psw;
	if (psh != sh)
	   dh = (dh * sh) / psh;
	if ((dw <= 0) || (dh <= 0) || (sw <= 0) || (sh <= 0))
	  {
	     return;
	  }
	/* clip output coords to clipped input coords */
	psx = dx;
	psy = dy;
	psw = dw;
	psh = dh;
	x2 = sx;
	y2 = sy;
	CLIP(dx, dy, dw, dh, 0, 0, im_dst->w, im_dst->h);
	if ((dw <= 0) || (dh <= 0) || (sw <= 0) || (sh <= 0))
	  {
	     return;
	  }
	if (psx != dx)
	   sx += ((dx - psx) * ssw) / abs(ddw);
	if (psy != dy)
	   sy += ((dy - psy) * ssh) / abs(ddh);
	if (psw != dw)
	   sw = (sw * dw) / psw;
	if (psh != dh)
	   sh = (sh * dh) / psh;
	dxx = dx - psx;
	dyy = dy - psy;
	dxx += (x2 * abs(ddw)) / ssw;
	dyy += (y2 * abs(ddh)) / ssh;

	if ((dw > 0) && (sw == 0))
	   sw = 1;
	if ((dh > 0) && (sh == 0))
	   sh = 1;
	/* do a second check to see if we now have invalid coords */
	/* don't do anything if we have a 0 width or height image to render */
	/* if the input rect size < 0 don't render either */
	if ((dw <= 0) || (dh <= 0) || (sw <= 0) || (sh <= 0))
	  {
	     return;
	  }
	/* if we are scaling the image at all make a scaling buffer */
	if (!((sw == dw) && (sh == dh) && (ddw > 0) && (ddh > 0)))
	  {
	     scaleinfo = __imlib_CalcScaleInfo(im_src, ssw, ssh, ddw, ddh, aa);
	     if (!scaleinfo) return;
	  }
	else
	  {
	     if (!IMAGE_HAS_ALPHA(im_dst))
		merge_alpha = 0;
	     if (!IMAGE_HAS_ALPHA(im_src))
	       {
		  rgb_src = 1;
		  if (merge_alpha)
		     blend = 1;
		  else
		     blend = 0;
	       }
	     __imlib_BlendRGBAToData(im_src->data, im_src->w, im_src->h,
				     im_dst->data, im_dst->w, im_dst->h,
				     ssx, ssy,
				     ddx, ddy,
				     ssw, ssh, blend, merge_alpha, cm, op, rgb_src);
	     return;
	  }
	/* if we are scaling the image at all make a scaling buffer */
	/* allocate a buffer to render scaled RGBA data into */
	buf = malloc(dw * LINESIZE * sizeof(DATA32));
	if (!buf)
	  {
	     __imlib_FreeScaleInfo(scaleinfo);
	     return;
	  }
	/* setup h */
	h = dh;
	if (!IMAGE_HAS_ALPHA(im_dst))
	   merge_alpha = 0;
	if (!IMAGE_HAS_ALPHA(im_src))
	  {
	     rgb_src = 1;
	     if (merge_alpha)
		blend = 1;
	     else
		blend = 0;
	  }
	/* scale in LINESIZE Y chunks and convert to depth*/
#ifdef DO_MMX_ASM
	do_mmx = __imlib_get_cpuid() & CPUID_MMX;
#endif
	for (y = 0; y < dh; y += LINESIZE)
	  {
	     hh = LINESIZE;
	     if (h < LINESIZE)
		hh = h;
	     /* scale the imagedata for this LINESIZE lines chunk of image */
	     if (aa)
	       {
#ifdef DO_MMX_ASM
		  if (do_mmx)
		     __imlib_Scale_mmx_AARGBA(scaleinfo, buf, dxx, dyy + y,
					      0, 0, dw, hh, dw, im_src->w);
		  else
#endif
		  if (IMAGE_HAS_ALPHA(im_src))
		     __imlib_ScaleAARGBA(scaleinfo, buf, dxx, dyy + y,
					 0, 0, dw, hh, dw, im_src->w);
		  else
		     __imlib_ScaleAARGB(scaleinfo, buf, dxx, dyy + y,
					0, 0, dw, hh, dw, im_src->w);
	       }
	     else
		__imlib_ScaleSampleRGBA(scaleinfo, buf, dxx, dyy + y,
					0, 0, dw, hh, dw);

	     __imlib_BlendRGBAToData(buf, dw, hh,
				     im_dst->data, im_dst->w,
				     im_dst->h,
				     0, 0, dx, dy + y, dw, dh,
				     blend, merge_alpha, cm, op, rgb_src);
	     h -= LINESIZE;
	  }
	/* free up our buffers and point tables */
	free(buf);
	__imlib_FreeScaleInfo(scaleinfo);
     }
}
