#ifndef __BLEND
#define __BLEND 1

/* FIXME: endian dependant */
#define READ_RGB(p, r, g, b)  \
   (r) = ((DATA8 *)p)[2]; \
   (g) = ((DATA8 *)p)[1]; \
   (b) = ((DATA8 *)p)[0];

#define READ_ALPHA(p, a) \
   (a) = ((DATA8 *)p)[3];

#define READ_RGBA(p, r, g, b, a) \
   (a) = ((DATA8 *)p)[3]; \
   (r) = ((DATA8 *)p)[2]; \
   (g) = ((DATA8 *)p)[1]; \
   (b) = ((DATA8 *)p)[0];

#define WRITE_RGB(p, r, g, b) \
   ((DATA8 *)p)[2] = r; \
   ((DATA8 *)p)[1] = g; \
   ((DATA8 *)p)[0] = b;

#define WRITE_RGB_PRESERVE_ALPHA(p, r, g, b) \
   ((DATA8 *)p)[2] = r; \
   ((DATA8 *)p)[1] = g; \
   ((DATA8 *)p)[0] = b;

#define WRITE_RGBA(p, r, g, b, a) \
   ((DATA8 *)p)[3] = a; \
   ((DATA8 *)p)[2] = r; \
   ((DATA8 *)p)[1] = g; \
   ((DATA8 *)p)[0] = b;


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
tmp = ((c) - (cc)) * (a); \
nc = (cc) + ((tmp + (tmp >> 8) + 0x80) >> 8);

#define ADD_COLOR_WITH_ALPHA(a, nc, c, cc) \
tmp = (cc) + (((c) * (a)) >> 8); \
SATURATE_UPPER(nc, tmp);

#define ADD_COLOR(nc, c, cc) \
tmp = (cc) + (c); \
SATURATE_UPPER(nc, tmp);

#define SUB_COLOR_WITH_ALPHA(a, nc, c, cc) \
tmp = (cc) - (((c) * (a)) >> 8); \
SATURATE_LOWER((nc), (tmp));

#define SUB_COLOR(nc, c, cc) \
tmp = (cc) - (c); \
SATURATE_LOWER(nc, tmp);

#define RESHADE_COLOR_WITH_ALPHA(a, nc, c, cc) \
tmp = (cc) + ((((c) - 127) * (a)) >> 7); \
SATURATE_BOTH(nc, tmp);

#define RESHADE_COLOR(nc, c, cc) \
tmp = (cc) + (((c) - 127) << 1); \
SATURATE_BOTH(nc, tmp);

#define BLEND(r1, g1, b1, a1, dest) \
READ_RGBA(dest, rr, gg, bb, aa); \
BLEND_COLOR(a1, nr, r1, rr); \
BLEND_COLOR(a1, ng, g1, gg); \
BLEND_COLOR(a1, nb, b1, bb); \
SATURATE_UPPER(na, (a1) + aa); \
WRITE_RGBA(dest, nr, ng, nb, na);

#define BLEND_ADD(r1, g1, b1, a1, dest) \
READ_RGBA(dest, rr, gg, bb, aa); \
ADD_COLOR_WITH_ALPHA(a1, nr, r1, rr); \
ADD_COLOR_WITH_ALPHA(a1, ng, g1, gg); \
ADD_COLOR_WITH_ALPHA(a1, nb, b1, bb); \
SATURATE_UPPER(na, (a1) + aa); \
WRITE_RGBA(dest, nr, ng, nb, na);

#define BLEND_SUB(r1, g1, b1, a1, dest) \
READ_RGBA(dest, rr, gg, bb, aa); \
SUB_COLOR_WITH_ALPHA(a1, nr, r1, rr); \
SUB_COLOR_WITH_ALPHA(a1, ng, g1, gg); \
SUB_COLOR_WITH_ALPHA(a1, nb, b1, bb); \
SATURATE_UPPER(na, (a1) + aa); \
WRITE_RGBA(dest, nr, ng, nb, na);

#define BLEND_RE(r1, g1, b1, a1, dest) \
READ_RGBA(dest, rr, gg, bb, aa); \
RESHADE_COLOR_WITH_ALPHA(a1, nr, r1, rr); \
RESHADE_COLOR_WITH_ALPHA(a1, ng, g1, gg); \
RESHADE_COLOR_WITH_ALPHA(a1, nb, b1, bb); \
WRITE_RGBA(dest, nr, ng, nb, na);

enum _imlibop
{
   OP_COPY,
   OP_ADD,
   OP_SUBTRACT,
   OP_RESHADE
};

typedef enum _imlibop ImlibOp;

typedef void (*ImlibBlendFunction)(DATA32*, int, DATA32*, int, int, int,
				   ImlibColorModifier *);

ImlibBlendFunction
__imlib_GetBlendFunction(ImlibOp op, char merge_alpha, char blend,
			 ImlibColorModifier * cm);
void
__imlib_BlendImageToImage(ImlibImage *im_src, ImlibImage *im_dst,
                          char aa, char blend, char merge_alpha,
                          int ssx, int ssy, int ssw, int ssh,
                          int ddx, int ddy, int ddw, int ddh,
			  ImlibColorModifier *cm, ImlibOp op);
void
__imlib_BlendRGBAToData(DATA32 *src, int src_w, int src_h, DATA32 *dst,
			int dst_w, int dst_h, int sx, int sy, int dx, int dy,
			int w, int h, char blend, char merge_alpha,
			ImlibColorModifier *cm, ImlibOp op);
#endif
