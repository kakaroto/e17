#ifndef __BLEND
#define __BLEND 1
void
__imlib_BlendRGBAToData(DATA32 *src, int src_w, int src_h, DATA32 *dst,
			int dst_w, int dst_h, int sx, int sy, int dx, int dy,
			int w, int h, char dalpha);
void
__imlib_BlendRGBAToRGB(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump,
		       int w, int h);
void
__imlib_BlendRGBAToRGBA(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump,
			int w, int h);

void
__imlib_CopyRGBAToRGBA(DATA32 *src, int src_jump, DATA32 *dst, int dst_jump,
                       int w, int h);
#endif
