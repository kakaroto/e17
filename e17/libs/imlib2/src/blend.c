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

/*
 * 1) Basic Saturation - 8 bit unsigned
 *
 * The add, subtract, and reshade operations generate new color values that may
 * be out of range for an unsigned 8 bit quantity.  Therefore, we will want to
 * saturate the values into the range [0, 255].  Any value < 0 will become 0,
 * and any value > 255 will become 255.  Or simply:
 *
 *   saturated = (value < 0) ? 0 : ((value > 255) ? 255 : value)
 *
 * Of course the above isn't the most efficient means of saturating.  Sometimes
 * due to the nature of a calculation, we know we only need to saturate from
 * above (> 255) or just from below (< 0).  Or simply:
 *
 *   saturated = (value < 0)   ?   0 : value
 *   saturated = (value > 255) ? 255 : value
 *
 * 2) Alternate Forms of Saturation
 *
 * The methods of saturation described above use testing/branching operations,
 * which are not necessarily efficient on all platforms.  There are other means
 * of performing saturation using just simple arithmetic operations
 * (+, -, >>, <<, ~).  A discussion of these saturation techniques follows.
 *
 * A) Saturation in the range [0, 512), or "from above".
 *
 * Assuming we have an integral value in the range [0, 512), the following
 * formula evaluates to either 0, or 255:
 *
 *    (value & 255) - ((value & 256) >> 8)
 *
 * This is easy to show.  Notice that if the value is in the range [0, 256)
 * the 9th bit is 0, and we get (0 - 0), which is 0.   And if the value is in
 * the range [256, 512) the 9th bit is 1, and we get (256 - 1), which is 255.
 *
 * Now, using the above information and the fact that assigning an integer to
 * an 8 bit unsigned value will truncate to the lower 8 bits of the integer,
 * the following properly saturates:
 *
 *    8bit_value = value | (value & 256) - ((value & 256) >> 8)
 *
 * To prove this to yourself, just think about what the lower 8 bits look like
 * in the ranges [0, 256) and [256, 512).  In particular, notice that the value
 * in the range [0, 256) are unchanged, and values in the range [256, 512)
 * always give you 255.  Just what we want!
 *
 * B) Saturation in the range (-256, 256), or "from below".
 *
 * Assuming we have an integral value in the range (-256, 256), the following
 * formula evaluates to either 0, or -1:
 *
 *   ~(value >> 8)
 *
 * Here's why.  If the value is in the range [0, 256), then shifting right by
 * 8 bits gives us all 0 bits, or 0.  And thus inverting the bits gives all
 * 1 bits, which is -1.  If the value is in the range (-256, 0), then the 9th
 * bit and higher bits are all 1.  So, when we shift right by 8 bits (with
 * signed extension), we get a value with all 1 bits.  Which when inverted is
 * all 0 bits, or 0.
 *
 * Now, using the above information the following properly saturates:
 *
 *    8bit_value = value & (~(value >> 8))
 *
 * To prove this to yourself, noticed that values in the range (-256, 0) will
 * always be AND'd with 0, and thus map to 0.   Further, values in the range
 * [0, 256) will always be AND'd with a value that is all 1 bits, and thus
 * be unchanged.  Just what we want!
 *
 * C) Saturation in the range (-256, 512), or "from above and below".
 *
 * The short of it is the following works:
 *
 *    8bit_value = (tmp | ((tmp & 256) - ((tmp & 256) >> 8))) & (~(tmp >> 9))
 *
 * We leave it to the reader to prove.  Looks very similar to the techniques
 * used above, eh? :)
 */

/* Saturate values in the range [0, 512) */
#define SATURATE_UPPER(nc, v) \
   tmp = (v);                 \
   nc = (tmp | ((tmp & 256) - ((tmp & 256) >> 8)));

/* Saturate values in the range (-256, 256) */
#define SATURATE_LOWER(nc, v) \
   tmp = (v);                 \
   nc = tmp & (~(tmp >> 8));

/* Saturate values in the range (-256, 512) */
#define SATURATE_BOTH(nc, v) \
   tmp = (v);                \
   nc = (tmp | ((tmp & 256) - ((tmp & 256) >> 8))) & (~(tmp >> 9));

/*
 * 1) Operations
 *
 * There are 4 operations supported:
 *
 *    Copy, Add, Subtract, and Reshade
 *
 * For each operation there are 3 different variations that can be made:
 *
 *   a) Use "blend" or "copy" in the calculations?  A "blend" uses the alpha
 *      value of the source pixel to lighten the source pixel values.  Where
 *      as "copy" ignores the alpha value and uses the raw source pixel values.
 *   b) Include source alpha in the calculation for new destination alpha?
 *      If source alpha is not used, then destination alpha is preserved.
 *      If source alpha is used, a "copy" sets the new alpha to the source
 *      alpha, and a "blend" adds them together (with saturation).
 *   c) Should the source pixels be passed through a color modifier before the
 *      calculations are performed?
 *
 * All together we have 4*2*2*2 = 32 combinations.
 *
 * 2) Copy operation
 *
 * The "copy" version of this operation copies the source image onto the
 * destination image.
 *
 * The "blend" version of this operation blends the source image color 'c' with
 * the destination image color 'cc' using 'a' (in the range [0, 1]) according
 * to the following formula.  Also notice that saturation is not needed for
 * this calculation, the output is in the range [0, 255]:
 *
 *    nc = c * alpha + (1 - alpha) * cc
 *       = c * alpha - cc * alpha + cc
 *       = (c - cc) * alpha + cc;
 *
 * A discussion of how we're calculating this value follows:
 *
 * We're using 'a', an integer, in the range [0, 255] for alpha (and for 'c'
 * and 'cc', BTW).  Therefore, we need to slightly modify the equation to take
 * that into account.  To get into the range [0, 255] we need to divide 'a'
 * by 255:
 *
 *    nc = ((c - cc) * a) / 255 + cc
 *
 * Notice that it is faster to divide by 256 (bit shifting), however without a
 * fudge factor 'x' to balance things this isn't horribly accurate.  So, let's
 * solve for 'x'.  The equality is:
 *
 *    ((c - cc) * a) / 256 + cc + x = ((c - cc) * a) / 255 + cc
 *
 * The 'cc' terms cancel, and multiply both sides by 255*256 to remove the
 * fractions:
 *
 *    ((c - cc) * a) * 255 + 255 * 256 * x = ((c - cc) * a) * 256
 *
 * Get the 'x' term alone:
 *
 *    255 * 256 * x = ((c - cc) * a)
 *
 * Divide both sides by 255 * 256 to solve for 'x':
 *
 *    x = ((c - cc) * a) / (255 * 256)
 *
 * And putting 'x' back into the equation we get:
 *
 *    nc = ((c - cc) * a) / 256 + cc + ((c - cc) * a) / (255 * 256)
 *
 * And if we let 'tmp' represent the value '(c - cc) * a', and do a little
 * regrouping we get:
 *
 *    nc = tmp / 256 + tmp / (255 * 256) + cc
 *       = (tmp + tmp / 255) / 256 + cc
 *
 * We'll be using integer arithmetic, and over the range of values tmp takes
 * (in [-255*255, 255*255]) the term tmp/(255*256) is pretty much the same as
 * tmp/(256*256).  So we get:
 *
 *    nc = (tmp + tmp / 256) / 256 + cc
 *
 * And because the division of the sum uses integer arithmetic, it always
 * rounds up/down even if that isn't the "best" choice.  If we add .5 to the
 * sum, we can get standard rounding:  Like so:
 *
 *    nc = (tmp + tmp / 256 + 128) / 256 + cc
 *
 * 3) Add operation
 *
 * The "copy" version of this operation sums the source image pixel values
 * with the destination image pixel values, saturating at 255 (from above).
 *
 * The "blend" version of this operation sums the source image pixel values,
 * after taking into account alpha transparency (e.g. a percentage), with the
 * destination image pixel values, saturating at 255 (from above).
 *
 * 4) Subtract operation
 *
 * This operation is the same as the Add operation, except the source values
 * are subtracted from the destination values (instead of added).  Further,
 * the result must be saturated at 0 (from below).
 *
 * 5) Reshade operation
 *
 * This operation uses the source image color values to lighten/darken color
 * values in the destination image using the following formula:
 *
 *    nc = cc + ((c - middle_value) * 2 * alpha)
 *
 * Recall our pixel color and alpha values are in the range [0, 255].  So, the
 * "blend" version of this operation can be calculated as:
 *
 *    nc = cc + ((c - 127) * 2 * (a / 255))
 *
 * And in an integer arithmetic friendly form is:
 *
 *    nc = cc + (((c - 127) * a) >> 7)
 *
 * The "copy" version of this operation treats alpha as 1.0 (or a/255), and in
 * integer arithmetic friendly form is:
 *
 *    nc = cc + ((c - 127) << 1)
 *
 * Notice the color values created by this operation are in the range
 * (-256, 512), and thus must be saturated at 0 and 255 (from above and below).
 */

#define BLEND_COLOR(a, nc, c, cc) \
   tmp = (c - cc) * a;            \
   nc = cc + ((tmp + (tmp >> 8) + 0x80) >> 8);

#define ADD_COLOR_WITH_ALPHA(a, nc, c, cc) \
   tmp = cc + ((c * a) >> 8);              \
   SATURATE_UPPER(nc, tmp);

#define ADD_COLOR(nc, c, cc) \
   tmp = cc + c;             \
   SATURATE_UPPER(nc, tmp);

#define SUB_COLOR_WITH_ALPHA(a, nc, c, cc) \
   tmp = cc - ((c * a) >> 8);              \
   SATURATE_LOWER(nc, tmp);

#define SUB_COLOR(nc, c, cc) \
   tmp = cc - c;             \
   SATURATE_LOWER(nc, tmp);

#define RESHADE_COLOR_WITH_ALPHA(a, nc, c, cc) \
   tmp = cc + (((c - 127) * a) >> 7);          \
   SATURATE_BOTH(nc, tmp);

#define RESHADE_COLOR(nc, c, cc) \
   tmp = cc + ((c - 127) << 1);  \
   SATURATE_BOTH(nc, tmp);



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
if (x < xx) {w += x; x = xx;} \
if (y < yy) {h += y; y = yy;} \
if ((x + w) > ww) {w = ww - x;} \
if ((y + h) > hh) {h = hh - y;}

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
