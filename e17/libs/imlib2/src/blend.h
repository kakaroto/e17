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
