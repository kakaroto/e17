#ifndef __BLEND
#define __BLEND 1

enum _imlibop
{
   OP_COPY,
   OP_ADD,
   OP_SUBTRACT,
   OP_RESHADE
};

typedef enum _imlibop ImlibOp;

void
__imlib_BlendImageToImage(ImlibImage *im_src, ImlibImage *im_dst,
                          char aa, char blend, char merge_alpha,
                          int ssx, int ssy, int ssw, int ssh,
                          int ddx, int ddy, int ddw, int ddh,
			  ImlibColorModifier *cm, ImlibOp op);
void
__imlib_BlendRGBAToData(DATA32 *src, int src_w, int src_h, DATA32 *dst,
			int dst_w, int dst_h, int sx, int sy, int dx, int dy,
			int w, int h, char dalpha, ImlibColorModifier *cm,
			ImlibOp op);
void
__imlib_BlendRGBAToRGB(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump,
		       int w, int h);
void
__imlib_BlendRGBAToRGBA(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump,
			int w, int h);
void
__imlib_CopyRGBAToRGB(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump,
                       int w, int h);
void
__imlib_CopyRGBAToRGBA(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump,
                       int w, int h);
void
__imlib_AddBlendRGBAToRGB(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump,
		       int w, int h);
void
__imlib_AddBlendRGBAToRGBA(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump,
			int w, int h);
void
__imlib_AddCopyRGBAToRGB(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump,
                       int w, int h);
void
__imlib_AddCopyRGBAToRGBA(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump,
                       int w, int h);
void
__imlib_SubBlendRGBAToRGB(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump,
		       int w, int h);
void
__imlib_SubBlendRGBAToRGBA(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump,
			int w, int h);
void
__imlib_SubCopyRGBAToRGB(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump,
                       int w, int h);
void
__imlib_SubCopyRGBAToRGBA(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump,
                       int w, int h);
void
__imlib_ReBlendRGBAToRGB(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump,
		       int w, int h);
void
__imlib_ReBlendRGBAToRGBA(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump,
			int w, int h);
void
__imlib_ReCopyRGBAToRGB(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump,
                       int w, int h);
void
__imlib_ReCopyRGBAToRGBA(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump,
                       int w, int h);
void
__imlib_BlendRGBAToRGB(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump,
		       int w, int h);
void
__imlib_BlendRGBAToRGBA(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump,
			int w, int h);
void
__imlib_BlendRGBAToRGBCmod(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump,
		       int w, int h, ImlibColorModifier *cm);
void
__imlib_BlendRGBAToRGBACmod(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump,
			int w, int h, ImlibColorModifier *cm);
void
__imlib_CopyRGBAToRGBCmod(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump,
                       int w, int h, ImlibColorModifier *cm);
void
__imlib_CopyRGBAToRGBACmod(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump,
                       int w, int h, ImlibColorModifier *cm);
void
__imlib_AddBlendRGBAToRGBCmod(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump,
		       int w, int h, ImlibColorModifier *cm);
void
__imlib_AddBlendRGBAToRGBACmod(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump,
			int w, int h, ImlibColorModifier *cm);
void
__imlib_AddCopyRGBAToRGBCmod(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump,
                       int w, int h, ImlibColorModifier *cm);
void
__imlib_AddCopyRGBAToRGBACmod(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump,
                       int w, int h, ImlibColorModifier *cm);
void
__imlib_SubBlendRGBAToRGBCmod(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump,
		       int w, int h, ImlibColorModifier *cm);
void
__imlib_SubBlendRGBAToRGBACmod(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump,
			int w, int h, ImlibColorModifier *cm);
void
__imlib_SubCopyRGBAToRGBCmod(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump,
                       int w, int h, ImlibColorModifier *cm);
void
__imlib_SubCopyRGBAToRGBACmod(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump,
                       int w, int h, ImlibColorModifier *cm);
void
__imlib_ReBlendRGBAToRGBCmod(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump,
		       int w, int h, ImlibColorModifier *cm);
void
__imlib_ReBlendRGBAToRGBACmod(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump,
			int w, int h, ImlibColorModifier *cm);
void
__imlib_ReCopyRGBAToRGBCmod(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump,
                       int w, int h, ImlibColorModifier *cm);
void
__imlib_ReCopyRGBAToRGBACmod(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump,
                       int w, int h, ImlibColorModifier *cm);
#endif
