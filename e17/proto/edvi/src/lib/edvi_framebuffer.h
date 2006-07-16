#ifndef __EDVI_FRAMEBUFFER_H__
#define __EDVI_FRAMEBUFFER_H__


Edvi_Framebuffer *edvi_framebuffer_new (int width,
                                        int height,
                                        int aa);

void              edvi_framebuffer_delete (Edvi_Framebuffer *framebuffer);

void              edvi_framebuffer_clear (Edvi_Framebuffer *framebuffer);


#endif /* __EDVI_FRAMEBUFFER_H__ */
